/*
 * @class Resampler
 *
 * @author Grzegorz Skołyszewski skolyszewski.grzegorz@gmail.com (Resampler) new 100%
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


#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cmath>

/**
 * @class Resampler
 * @brief Resampling used to accumulate sampling frequency drifts in playing from file (to any destination) mode.
 *
 * Two resampling (interpolation) modes: Nearest Neighbor (zero order), Linear (first order).
 *
 * @author Grzegorz Skołyszewski skolyszewski.grzegorz@gmail.com
 * @copyright Public domain
 */
class Resampler {
public:
    /**
    * @brief enum for resampling type
    *
    */
    enum resampling_type
    {
        NN          = 0,
        LINEAR,
        PCHIP,
        SPLINE
    };
    /**
    * Resampler constructor
    * @param[in] type Resampler type, enum specifying type of resampling (interpolation used). See @resampling_type
    * @param[in] inner_buffer_size Size of inner buffer used to store resampled data
    */
    Resampler(resampling_type type, size_t inner_buffer_size);
    virtual ~Resampler();

    /**
    * Main Resampler method, takes data from source buffer of specified length and ratio, then resamples it into inner buffer.
    * Depending on conv_type_, it invokes right resampling method. This is actually only a wrapper, all the processing is done with methods invoked by this one.
    * @param[in] *src_buf Pointer to source data buffer
    * @param[in] input_length Length of data from source buffer to read and process
    * @param[in] ratio Resampling ratio (calculated from fs drifts)
    * @param[out] Returns Number of samples written into inner buffer
    */
    size_t ResampleIntoBuffer(float *src_buf, size_t input_length, float ratio);
    /**
    * Copies data from inner buffer to destination buffer. Number of samples copied is specified by copy_length
    * @param[in] *dst_buffer Pointer to destination buffer
    * @param[in] copy_length Length of data to copy from inner buffer to destination buffer
    * @param[out] Returns Number of samples copied into destination buffer
    */
    size_t CopyFromBuffer(float *dst_buffer, size_t copy_length);
    /**
    * Returns number of samples stored in inner buffer
    * @param[out] Returns number of samples stored in inner buffer
    */
    size_t DataStoredInBuffer();
    /**
    * Returns free space in buffer (amount of space that we can write into without overwriting old data or going out buffer boundary).
    * @param[out] Returns free space in buffer
    */
    size_t FreeSpaceInBuffer();

#ifndef GOOGLE_UNIT_TEST
private:
#endif
    /**
    * Nearest Neighbor resampling processing method. Samples are interpolated with the value of nearest lying sample. Very fast, needs very good SNR to produce reliable samples.
    * @param[in] *src_buf Pointer to source data buffer
    * @param[in] input_length Length of data from source buffer to read and process
    * @param[in] ratio Resampling ratio (calculated from fs drifts)
    * @param[out] Returns Number of samples written into inner buffer
    */
    size_t ResampleNn(float *src_buf, size_t input_length, float ratio);
    /**
    * Linear resampling processing method. Samples are interpolated linearly. Fast (slower than NN), reliable.
    * @param[in] *src_buf Pointer to source data buffer
    * @param[in] input_length Length of data from source buffer to read and process
    * @param[in] ratio Resampling ratio (calculated from fs drifts)
    * @param[out] Returns Number of samples written into inner buffer
    */
    size_t ResampleLinear(float *src_buf, size_t input_length, float ratio);
    /**
    * Interpolates value between samples a and b depending on the distance from them. If the interpolated sample is closer to sample A, it takes it's value, same with sample B.
    * @param[in] a Value of sample a 
    * @param[in] b Value of sample b
    * @param[in] distance Distance from sample a (distance from sample a to b is equal 1)
    * @param[out] Returns value of seeked sample
    */
    float InterpolateNn(float a, float b, float distance);
    /**
    * Interpolates value between samples a and b depending on the distance from them. Interpolation is linear ( |b-a| * distance) where distance if value between (0..1).
    * @param[in] a Value of sample a 
    * @param[in] b Value of sample b
    * @param[in] distance Distance from sample a (distance from sample a to b is equal 1)
    * @param[out] Returns value of seeked sample
    */
    float InterpolateLinear(float a, float b, float distance);
    /**
    * Rounds down to closest even number. I.e. invoked on 3.14 returns 2.
    * @param[in] a Input number
    * @param[out] Returns maximum even number lower than input
    */
    size_t RoundDownToEven(float a);

    /**
    * Can be used to manipulate interpolation type
    */
    resampling_type conv_type_;
    /**
    * Inner data buffer, should be at least 3 times longer than maximum Resampler input to assure stability
    */
    float *data_buffer_;
    /**
    * Debug boolean, if set to True, processing displays additional information to stdout
    */
    int debug;
    /**
    * Used store Q sample value that will be useful in the next iteration of Resampler processing
    */
    float previous_sample_Q_;
    /**
    * Used store I sample value that will be useful in the next iteration of Resampler processing
    */
    float previous_sample_I_;
    /**
    * Array to store sample positions that will be useful in the next iteration of Resampler processing
    */
    double previous_sample_position_[2];
    /**
    * Variable used to determine where the last useful sample produced by processing is in the buffer. All the samples from the beggining of data_buffer_ to pointer_ are important and must be used.
    */
    size_t pointer_;
    /**
    * Length of inner buffer (data_buffer_)
    */
    size_t length_;
    /**
    * Indicates that this is first iteration of Resampler after object creation
    */
    int first_iteration_;


};

#endif /* RESAMPLER_H_ */
