/*
 * @class FileDataFeeder
 * @brief provides data to the system
 *
 * Reads data from file, executes basic DSP operations
 *
 * @author Paweł Szulc pawel_szulc@onet.pl
 * @author Wojciech Szmyd wojszmyd@gmail.com
 * @author Alicja Gegotek alicja.gegotek@gmail.com
 * @author Piotr Kmiecik piotr.kmiecik94@gmail.com
 * @author Grzegorz Skołyszewski skolyszewski.grzegorz@gmail.com 5%
 * @date 7 July 2015 - version 1.0 beta
 * @date 7 July 2016 - version 2.0 beta
 * @date 1 November 2016 - version 2.0
 * @date 7 July 2017 - version 3.0
 * @version 3.0
 * @copyright Copyright (c) 2015 Paweł Szulc, Wojciech Szmyd, Alicja Gegotek
 * @par License
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "file_data_feeder.h"
#include "../threading/blocking_queue.h"
#include "../data_format.h"

#ifdef __MACH__
#include "osx_compat.h"
#endif

// TODO: debug -> verbose

FileDataFeeder::FileDataFeeder(fileType_t file_type_, const char *file_name, size_t buf_s,
        uint32_t sample_rate, uint32_t carrier_freq, int number_of_bits, Resampler2::resampling_type type):AbstractDataFeeder(number_of_bits){

    s_rate_ = sample_rate;

    c_freq_ = carrier_freq;

    file_descriptor_ = open(file_name,O_RDONLY);

    inner_buff_size_ = buf_s;
    inner_buf_num_ = 1;

    resampler2_ = new Resampler2(type,inner_buff_size_*4);
    normalization_buffer_ = new float[buf_s];

    file_type = file_type_;
    size_of_IQ_sample = FileDataFeeder::SizeOfOneIQSample(file_type_);
    size_of_number = size_of_IQ_sample/2;

    file_buffer = new unsigned char[buf_s * size_of_number];
    buffer_to_process = new float[buf_s];
};

FileDataFeeder::~FileDataFeeder() {
    delete[] file_wrapper_buffer_;
    //delete resampling_buffer_;
    delete[] normalization_buffer_;
    delete resampler2_;
    delete[] file_buffer;
    delete[] buffer_to_process;
};

uint32_t FileDataFeeder::GetCenterFrequency(){
    return c_freq_;
};

uint32_t FileDataFeeder::GetSamplingFrequency(){
    return s_rate_;
};

uint32_t FileDataFeeder::SetCenterFrequency(uint32_t fc){
    c_freq_ = fc;
    return c_freq_;
};

uint32_t FileDataFeeder::SetSamplingFrequency(uint32_t fs){
    s_rate_ = fs;
    return s_rate_;
};

void FileDataFeeder::ReadAsync(void *data_needed){

    __useconds_t SLEEP_TIME = 5;
    struct timeval play_start_time, play_end_time;
    data_feeder_ctx_t *params = static_cast<data_feeder_ctx_t*>(data_needed);
    BlockingQueue<int> *event_queue = reinterpret_cast<BlockingQueue<int>*>(params->event_queue);
    pthread_cond_t *pointer_changed_cond = reinterpret_cast<pthread_cond_t*>(params->pointer_changed_cond);

    while(1){
        pthread_mutex_lock(params->lock_buffer);

        if(params->data_stored) {
            while (!params->write_ready && !params->finish_rtl_process) {
                pthread_cond_wait(pointer_changed_cond,  params->lock_buffer);
            }
        }

        params->write_ready = false;
        if (params->finish_rtl_process) {
            return;
        }
        // if we have enough data in Resampler2's buffer, we can read it before reading from file (see Resampler2 behavior)
        if (!EnoughDataInBuffer(params->block_size)) {
//            if(verbose)
//                printf("Not enough data in resampling buffer, reading from file\n");

            size_t number_written_samplesIQ = ReadNumberOfSamplesIQ(params->block_size/2);

            if(number_written_samplesIQ <= 0) {
                params->data_stored = true;
                event_queue->push(params->thread_id);
                running = 0;
                pthread_mutex_unlock(params->lock_buffer);
                if (verbose)
                    std::cout << "NO MORE DATA IN FILE\n STOPPING\n";
                fflush(stdout);
                return;
            }

            CopySamplesToBuffer(number_written_samplesIQ);
            float ratio = PickRatio(params->block_size);

//            if (verbose){
//                printf("Read from file, resampling signal with ratio: %1.8f\n",ratio);
//            }
            // if ratio == 1.0 we dont need to copy data into resampling buffer, we just pass the data further and unlock the mutex so another thread can process the data
            if (ratio==1.0){
//                if (verbose){
//                    printf("Ratio: 1.0, copying buffers of length: %lu\n", params->block_size);
//                }
                CopyWithoutResampling(params,event_queue);
                pthread_mutex_unlock(params->lock_buffer);
                continue;
            }else{ // else if the ratio is not 1.0, we process data by resampling it into resampling buffer
                resampler2_->ResampleIntoBuffer(buffer_to_process,number_written_samplesIQ*2,ratio);
            }
        } 

        // if we have enoguh data in resampling buffer (we just wrote it or we produced it earlier) we can just write it out to destination buffer
        if(EnoughDataInBuffer(params->block_size)){
            WriteResampledOut(params, event_queue);
        }else{
            params->data_stored = false;
        }

        pthread_mutex_unlock(params->lock_buffer);
    }   // end of while - when finish_rtl_process = true
};

inline void FileDataFeeder::SetDelay(timeval play_start_time, timeval play_end_time){
    double start = (play_start_time.tv_sec + (play_start_time.tv_usec/1000000.0))*1000;
    double end = (play_end_time.tv_sec + (play_end_time.tv_usec/1000000.0))*1000;
    double processing_time = 40000 - ((end - start) *  1000);
    if(processing_time > 0)
        read_delay = (__useconds_t) processing_time;
};

inline size_t FileDataFeeder::ReadFromFile(size_t block_size) {
    inner_buff_size_ = block_size;
    return read(file_descriptor_, file_wrapper_buffer_, inner_buff_size_);
};

inline bool FileDataFeeder::EnoughDataReadFromFile(size_t number_written, data_feeder_ctx_t *params, BlockingQueue<int> *event_queue){
    bool should_continue = true;
    if(number_written<inner_buff_size_){
        params->data_stored = true;
        event_queue->push(params->thread_id);
        running = 0;
        if (verbose) {
            std::cout << "NO MORE DATA IN FILE\n STOPPING\n";
        }
        fflush(stdout);
        pthread_mutex_unlock(params->lock_buffer);
        should_continue = false;
    }
    return should_continue;
};

inline float FileDataFeeder::PickRatio(size_t block_size){
//    if(verbose) {
//        printf("current fs offset: %f\n", current_fs_offset_);
//    }
    float ratio = 1.0 - current_fs_offset_/1000000.0;
    float block_size_float = static_cast<float>(block_size);
    float number_of_probes = ratio*block_size_float;

    if (number_of_probes>block_size_float-1.0 && number_of_probes<block_size_float+1.0)
        ratio=1.0;
    if (!do_handle_fs_)
        ratio=1.0;
    return ratio;
};

inline bool FileDataFeeder::EnoughDataInBuffer(size_t expected_amount){
    return resampler2_->DataStoredInBuffer()>=expected_amount;
};

inline void FileDataFeeder::WriteResampledOut(data_feeder_ctx_t *ptctx, BlockingQueue<int> *event_queue){
    resampler2_->CopyFromBuffer(ptctx->write_here, ptctx->block_size);
    previous_write_here_ = ptctx->write_here;
    ptctx->data_stored = true;
    event_queue->push(ptctx->thread_id);
};

inline void FileDataFeeder::CopyWithoutResampling(data_feeder_ctx_t *ptctx, BlockingQueue<int> *event_queue){
    memcpy(ptctx->write_here, buffer_to_process, ptctx->block_size*sizeof(float));
    previous_write_here_ = ptctx->write_here;
    ptctx->data_stored = true;
    event_queue->push(ptctx->thread_id);
}

void FileDataFeeder::Normalize(size_t data_size){
    // insert into output buffer
    // remove DC from real and image part
    // probes are normalized to +-1.0
    float real_mean = real_dc_rb_->Mean();
    float imag_mean = imag_dc_rb_->Mean();
    float real_sum = 0.0;
    float imag_sum = 0.0;
    unsigned char c1,c2;
    float f1,f2;
    for (size_t k = 0; k < data_size; k+=2){
        c1 = file_buffer[k];
        c2 = file_buffer[k+1];
        f1 = static_cast<float>(c1) - 127.0;
        f2 = static_cast<float>(c2) - 127.0;
        real_sum += f1;
        imag_sum += f2;
        *(buffer_to_process + k) = (f1-real_mean)/128.0;
        *(buffer_to_process + k+1) = (f2-imag_mean)/128.0;
    }

    real_dc_rb_->WriteNext(real_sum*2/data_size);
    imag_dc_rb_->WriteNext(imag_sum*2/data_size);
};

bool FileDataFeeder::EverythingOK(void){
    if (file_buffer==NULL){
        if (verbose)
            fprintf(stderr,"No file wrapper found\n");
        return false;
    }
    if (GetSamplingFrequency()==0){
        if (verbose)
            fprintf(stderr,"Sampling frequency not set\n");
        return false;
    }
    if (file_descriptor_<0){
        if (verbose)
            fprintf(stderr,"Can't read from file :(\n");
        return false;
    }
    return true;
};

void FileDataFeeder::HandleDrifts(float fc_drift, float fs_drift){
    current_fc_offset_ += fc_drift;
    if (do_handle_fs_)
        current_fs_offset_ += fs_drift;
};

long int FileDataFeeder::ReadNumberOfSamplesIQ(long int samples_number){
    long int number_of_read_bytes = read(file_descriptor_, file_buffer, samples_number * size_of_IQ_sample);
    return number_of_read_bytes/size_of_IQ_sample;
};

// ToDo: rafactore code to remove <template> and remove CopySamplesToBufferDecorator()
// ToDo: this scheme is inefficience, memcpy() is used to copy one byte (!)
// ToDo: switch should be in CopySamplesToBuffer()
void FileDataFeeder::CopySamplesToBuffer(size_t number_of_loaded_samples) {
    float f1,f2;
    switch (file_type) {
        case type_raw:
            Normalize(number_of_loaded_samples * 2);
            break;
        case type_uint8:
            for (long int k = 0; k < number_of_loaded_samples; k++) {
                f1 = static_cast<float>( file_buffer[k * size_of_IQ_sample]);
                f2 = static_cast<float>( file_buffer[k * size_of_IQ_sample + size_of_number]);
                *(buffer_to_process + k * 2) = f1 - (float) 127;
                *(buffer_to_process + k * 2 + 1) = f2 - (float) 127;
            }
            break;
        case type_int8:
            for (long int k = 0; k < number_of_loaded_samples; k++) {
                *(buffer_to_process + k * 2) = static_cast<float>( *((signed char *) &file_buffer[k * size_of_IQ_sample]));
                *(buffer_to_process + k * 2 + 1) = static_cast<float>( *((signed char *) &file_buffer[k * size_of_IQ_sample + size_of_number]));
            }
            break;
        case type_uint16:
            for (long int k = 0; k < number_of_loaded_samples; k++) {
                f1 = static_cast<float>( *((uint16_t *) &file_buffer[k * size_of_IQ_sample]));
                f2 = static_cast<float>( *((uint16_t *) &file_buffer[k * size_of_IQ_sample + size_of_number]));
                *(buffer_to_process + k * 2) = f1 - (float) 32767;
                *(buffer_to_process + k * 2 + 1) = f2 - (float) 32767;
            }
            break;
        case type_int16:
            for (long int k = 0; k < number_of_loaded_samples; k++) {
                *(buffer_to_process + k * 2) = static_cast<float>( *((int16_t *) &file_buffer[k * size_of_IQ_sample]));
                *(buffer_to_process + k * 2 + 1) = static_cast<float>( *((int16_t *) &file_buffer[k * size_of_IQ_sample + size_of_number]));
            }
            break;
        case type_float:
            for (long int k = 0; k < number_of_loaded_samples; k++) {
                 *(buffer_to_process + k * 2) = ( *((float*) &file_buffer[k * size_of_IQ_sample]));
                 *(buffer_to_process + k * 2 + 1) = ( *((float *) &file_buffer[k * size_of_IQ_sample + size_of_number]));
            }
            break;
        case type_double:
            for (long int k = 0; k < number_of_loaded_samples; k++) {
                *(buffer_to_process + k * 2) =  static_cast<float>( *((double*) &file_buffer[k * size_of_IQ_sample]));
                *(buffer_to_process + k * 2 + 1) = static_cast<float>( *((double *) &file_buffer[k * size_of_IQ_sample + size_of_number]));
            }
            break;
        default:
            fprintf(stderr,"CopySamplesToBuffer: Unsupported file type (!!!)\n");
    }
};

short FileDataFeeder::SizeOfOneIQSample(fileType_t fileType) {
    short sizeOfOneSample = 4;
    switch (fileType) {
        case type_raw:
            sizeOfOneSample = 2;
            break;
        case type_uint8:
            sizeOfOneSample = 2;
            break;
        case type_int8:
            sizeOfOneSample = 2;
            break;
        case type_uint16:
            sizeOfOneSample = 4;
            break;
        case type_int16:
            sizeOfOneSample = 4;
            break;
        case type_float:
            sizeOfOneSample = 8;
            break;
        case type_double:
            sizeOfOneSample = 16;
            break;
        default:
            fprintf(stderr,"SizeOfOneIQSample: Unsupported file type (!!!)\n");
    }

    return sizeOfOneSample;
}
