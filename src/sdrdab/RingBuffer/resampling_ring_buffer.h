﻿/**
 * @author: Paweł Szulc <pawel_szulc@onet.pl>
 * @date 7 July 2015 - version 1.0 beta
 * @date 7 July 2016 - version 2.0 beta
 * @date 1 November 2016 - version 2.0
 * @date 7 July 2017 - version 3.0
 * @version 3.0
 * @copyright Copyright (c) 2015 Paweł Szulc
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


#ifndef RESAMPLING_RING_BUFFER_H_
#define RESAMPLING_RING_BUFFER_H_

#include "ring_buffer.h"
#include "Resampler/resampler.h"
#include "Resampler/resampler2.h"


class ResamplingRingBuffer : public RingBuffer<float> {
    public:
    enum resample_quality
    {
        SRC_SINC_BEST_QUALITY		= 0,
        SRC_SINC_MEDIUM_QUALITY		= 1,
        SRC_SINC_FASTEST			= 2,
        SRC_ZERO_ORDER_HOLD			= 3,
        SRC_LINEAR					= 4,
    } ;
        ResamplingRingBuffer(resample_quality quality, size_t size, int channels);

        virtual ~ResamplingRingBuffer();

        size_t WriteResampledInto(float* source_buffer, size_t number_to_read, float ratio);
        //  size_t sWriteResampledInto(float* source_buffer, size_t number_to_read, float ratio);

    private:
        Resampler *resampler;
};

#endif /* RESAMPLING_RING_BUFFER_H_ */
