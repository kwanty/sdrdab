/*
 * @class FileDataFeeder
 * @brief provides data to the system
 *
 * Reads data from file, executes basic DSP operations
 *
 * @author Paweł Szulc <pawel_szulc@onet.pl>
 * @author Wojciech Szmyd <wojszmyd@gmail.com>
 * @date 7 July 2015
 * @version 1.0 beta
 * @copyright Copyright (c) 2015 Paweł Szulc, Wojciech Szmyd
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

static timespec lt;

void TimeHelper(){
	// !!! UNCOMMENT THIS TO IMPROVE PERFORMANCE !!!
	//    std::cout <<" ";
}

FileDataFeeder::FileDataFeeder(const char *file_name, size_t buf_s,
		uint32_t sample_rate, uint32_t carrier_freq){
	s_rate = sample_rate;

	c_freq = carrier_freq;

	file_descriptor = open(file_name,O_RDONLY);

	inner_buff_size = buf_s;
	inner_buf_num = 1;

	file_wrapper_buffer = new unsigned char[buf_s];

	res_buffer = new ResamplingRingBuffer(SRC_LINEAR,inner_buff_size*4,2);
//	res_buffer = new ResamplingRingBuffer(SRC_SINC_FASTEST,inner_buff_size*4,2);
};

FileDataFeeder::~FileDataFeeder() {
	delete[] file_wrapper_buffer;
	delete res_buffer;

};

uint32_t FileDataFeeder::GetFC(){
	return c_freq;
};

uint32_t FileDataFeeder::GetFS(){
	return s_rate;
};

uint32_t FileDataFeeder::SetFC(uint32_t fc){
	c_freq = fc;
	return c_freq;
};

uint32_t FileDataFeeder::SetFS(uint32_t fs){
	s_rate = fs;
	return s_rate;
};

void FileDataFeeder::ReadAsync(void *data_needed){

	clock_gettime(CLOCK_REALTIME, &lt);

	data_feeder_ctx_t *ptctx = static_cast<data_feeder_ctx_t*>(data_needed);

	while(1){
		timespec abs_time;
		clock_gettime(CLOCK_REALTIME, &abs_time);
		abs_time.tv_sec += 1; // chosen arbitrary, doesn't really matter - just making sure it won't block itself forever

		if (0==pthread_mutex_timedlock(ptctx->lock_buffer, &abs_time)){

			if (ptctx->finish_rtl_process) {
				return;
			}

			float *out_buffer = ptctx->write_here;

			if (out_buffer!=write_previous){

				// checking if got enough resampled data in ring buffer
				if (res_buffer->DataStored()>=ptctx->block_size){
					if(debug)
						printf("Got extra data: %d\n",static_cast<int>(res_buffer->DataStored()));
					size_t number_written = res_buffer->sReadFrom(out_buffer,ptctx->block_size);

					write_previous = out_buffer;

					Remodulate(out_buffer,number_written,current_fc_offset);

					ptctx->data_stored = true;

				} else {
					inner_buff_size = ptctx->block_size;
					size_t number_written = read(file_descriptor,file_wrapper_buffer,inner_buff_size);

					if(number_written<ptctx->block_size){
						ptctx->data_stored = true;
						running = 0;
						if (verbose)
							std::cout << "NO MORE DATA IN FILE\n STOPPING\n";
						fflush(stdout);
						pthread_mutex_unlock(ptctx->lock_buffer);
						return;
					}

					// insert into output buffer
					// remove DC from real and image part
					// probes are normalized to +-1.0
					float real_mean = real_dc_rb->Mean();
					float imag_mean = imag_dc_rb->Mean();
					float real_sum = 0.0;
					float imag_sum = 0.0;
					unsigned char c1,c2;
					float f1,f2;
					for (size_t k = 0; k < number_written; k+=2){
						c1 =file_wrapper_buffer[k];
						c2 =file_wrapper_buffer[k+1];
						f1 = static_cast<float>(c1) - 127.0;
						f2 = static_cast<float>(c2) - 127.0;
						real_sum += f1;
						imag_sum += f2;
						*(out_buffer + k) = (f1-real_mean)/128.0;
						*(out_buffer + k+1) = (f2-imag_mean)/128.0;
					}
					real_dc_rb->WriteNext(real_sum*2/number_written);
					real_dc_rb->WriteNext(imag_sum*2/number_written);

					float ratio = 1.0 - current_fs_offset/1000000.0;
					float out_no_probes = ratio*static_cast<float>(ptctx->block_size);
					if (out_no_probes>static_cast<float>(ptctx->block_size)-1.0 && out_no_probes<static_cast<float>(ptctx->block_size)+1.0)
						ratio=1.0;
					if (!do_handle_fs)
						ratio=1.0;
					if (debug)
						printf("Read from file, writing into ring buffer: %d\t with ratio: %1.8f\n",static_cast<int>(res_buffer->DataStored()),ratio);
					res_buffer->WriteResampledInto(out_buffer,number_written,ratio);
					if (res_buffer->DataStored()>=ptctx->block_size){
						if(debug)
							printf("Got enough data in ring buffer: %d\n",static_cast<int>(res_buffer->DataStored()));
						write_previous = out_buffer;
						size_t number_written = res_buffer->sReadFrom(out_buffer,ptctx->block_size);
						Remodulate(out_buffer,number_written,current_fc_offset);
						ptctx->data_stored = true;
					} else {
						ptctx->data_stored = false;
					}

				} // <!--normal read from file
			} else {
				// no new place to write into specified
				// we read from file - blocks aren't skipped

				usleep(5);
			}

			pthread_mutex_unlock(ptctx->lock_buffer);

		}else {
			// can't lock - wait a bit
			usleep(5);	// arbitrary chosen number - needs some testing
			// probably don't need to do anything more
		}

		TimeHelper();

	}	// end of while - when finish_rtl_process = true


	return;
};


void FileDataFeeder::StopProcessing(void){
	return;
};

bool FileDataFeeder::EverythingOK(void){
	if (file_wrapper_buffer==NULL){
		if (verbose)
			fprintf(stderr,"No file wrapper found\n");
		return false;
	}
	if (GetFS()==0){
		if (verbose)
			fprintf(stderr,"FS not set\n");
		return false;
	}

	if (file_descriptor<0){
		if (verbose)
			fprintf(stderr,"Can't read from file :(\n");
		return false;
	}

	return true;
}

void FileDataFeeder::HandleDrifts(float fc_drift, float fs_drift){


	current_fc_offset += fc_drift;

	if (do_handle_fs)
		current_fs_offset += fs_drift;
}

bool FileDataFeeder::FromFile(void){
	return true;
}

bool FileDataFeeder::FromDongle(void){
	return false;
}

void
FileDataFeeder::ResamplerType (int quality) {

	delete res_buffer;
	res_buffer = new ResamplingRingBuffer(quality,inner_buff_size*4,2);

}
