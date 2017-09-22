/*
 * @class Resampler2
 *
 * @author Grzegorz Skołyszewski skolyszewski.grzegorz@gmail.com (Resampler2)  new 100%
 *
 * @date 12 June 2017 - version 3.0
 * @version 3.0
 *
 * @copyright Copyright (c) 2017 Grzegorz Skołyszewski
 *
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

#include "resampler2.h"

Resampler2::Resampler2(resampling_type type, size_t inner_buffer_size):
    conv_type_(type),
    length_(inner_buffer_size),
    pointer_(0),
    first_iteration_(0),
    debug(0) {
        data_buffer_ = new float[inner_buffer_size];
        previous_sample_Q_ = 0.0;
        previous_sample_I_ = 0.0;
        previous_sample_position_[0] = 0.0;
        previous_sample_position_[1] = 0.0;
}

Resampler2::~Resampler2(){
    delete[] data_buffer_;

}

size_t Resampler2::ResampleIntoBuffer(float *src_buf, size_t input_length, float ratio){
    size_t resampled_data_count = 0;

    if(first_iteration_){
        previous_sample_Q_ = src_buf[0];
        previous_sample_I_ = src_buf[1];
        //first_iteration_ = 0;
    }

    switch(conv_type_) {
        case(0):
            resampled_data_count = ResampleNn(src_buf, input_length, ratio);
            break;
        case(1):
            resampled_data_count = ResampleLinear(src_buf, input_length, ratio);
            break;
        case(2):
            //PCHIP
            break;
        case(3):
            //SPLINE
            break;
    }
    return resampled_data_count;
}

size_t Resampler2::ResampleNn(float *src_buf, size_t input_length, float ratio){
    size_t desired_length = size_t((input_length/2)*ratio);
    
    if(desired_length * 2 > FreeSpaceInBuffer()){
        if(debug)
            printf("Not enough space in circular buffer, not writing into");
        return 0;
    }
    
    long double stime, dt;
    size_t rounded_stime;
    float * data_buffer_ptr = data_buffer_ + pointer_;
    size_t generated_samples = 0, used_samples = 0;
    dt = 1/ratio;

    stime = 0.0;
    
    
    while(rounded_stime+3 <= input_length){
        rounded_stime = RoundDownToEven(stime);
        data_buffer_ptr[0] = InterpolateNn(src_buf[rounded_stime], src_buf[rounded_stime + 2], (stime - rounded_stime) / 2);
        data_buffer_ptr[1] = InterpolateNn(src_buf[rounded_stime+1], src_buf[rounded_stime + 3], (stime - rounded_stime) / 2);
        generated_samples += 2;
        stime = generated_samples*dt;
        data_buffer_ptr += 2;
    }
    used_samples = rounded_stime;
    //printf("gs: %lu, us: %lu\n",generated_samples, used_samples);

    previous_sample_I_ = src_buf[input_length-2];
    previous_sample_Q_ = src_buf[input_length-1];
    pointer_ = pointer_ + generated_samples;
    first_iteration_ = 0;
    return generated_samples;
}

size_t Resampler2::ResampleLinear(float *src_buf, size_t input_length, float ratio){
    size_t desired_length = size_t((input_length/2)*ratio);
    
    if(desired_length * 2 > FreeSpaceInBuffer()){
        if(debug)
            printf("Not enough space in circular buffer, not writing into");
        return 0;
    }
    
    double stime, dt;
    size_t rounded_stime;
    float * data_buffer_ptr = data_buffer_ + pointer_;
    size_t generated_samples = 0, used_samples = 0;
    dt = 1/ratio;

    stime = 0.0;
    rounded_stime = RoundDownToEven(stime);


    while(rounded_stime+3 <= input_length){
        rounded_stime = RoundDownToEven(stime);
        data_buffer_ptr[0] = InterpolateLinear(src_buf[rounded_stime], src_buf[rounded_stime + 2], (stime - rounded_stime) / 2);
        data_buffer_ptr[1] = InterpolateLinear(src_buf[rounded_stime+1], src_buf[rounded_stime + 3], (stime - rounded_stime) / 2);
        generated_samples += 2;
        stime = generated_samples*dt;
        data_buffer_ptr += 2;
    }
        used_samples = rounded_stime;
    //printf("gs: %lu, us: %lu\n",generated_samples, used_samples);

    previous_sample_I_ = src_buf[rounded_stime + 1];
    previous_sample_Q_ = src_buf[rounded_stime + 2];
    pointer_ = pointer_ + generated_samples;
    first_iteration_ = 0;
    return generated_samples;
}

size_t Resampler2::CopyFromBuffer(float *dst_buffer, size_t copy_length){
    if(DataStoredInBuffer() < copy_length) // check if there's enough data in buffer
    {
        if(debug)
            printf("Not enough data in resampling buffer\n");
        return 0;
    }
    memcpy(dst_buffer, data_buffer_, sizeof(float)*(copy_length));
    // evaluate how many samples left in the buffer and copy them to the beginning of the buffer
    if(DataStoredInBuffer() > copy_length){
        memcpy(data_buffer_, data_buffer_ + copy_length, sizeof(float)*(pointer_ - copy_length));
        pointer_ = pointer_ - copy_length;
    }else{
        pointer_ = 0;
    }

    return copy_length;
}

size_t Resampler2::DataStoredInBuffer(){
    return pointer_;
}

inline size_t Resampler2::FreeSpaceInBuffer(){
    return length_ - pointer_;
}

inline float Resampler2::InterpolateLinear(float a, float b, float distance){
    return a + (distance * (b - a));
}

inline float Resampler2::InterpolateNn(float a, float b, float distance){
    return (distance <= 0.5) ? a : b;
}

inline size_t Resampler2::RoundDownToEven(float a){
    return size_t(a) & ~1;
}