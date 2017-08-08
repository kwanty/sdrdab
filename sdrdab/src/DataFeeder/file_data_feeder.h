/**
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


#ifndef FILEDATAFEEDER_H_
#define FILEDATAFEEDER_H_
#include "abstract_data_feeder.h"
#include <fcntl.h>

class FileDataFeeder : public AbstractDataFeeder {
  public:
/**
 * Create DataFeeder object to read from file
 * @param file_name file with data, each char is read as 0-255 number for further processing
 * @param buf_s size of inner buffer (probes number it can hold)
 * @param sample_rate sample rate of data saved in file
 * @param carrier_freq center frequency of data saved in file
 */
    FileDataFeeder(const char *file_name, size_t buf_s,
        uint32_t sample_rate, uint32_t carrier_freq);

    ~FileDataFeeder();

    virtual void StopProcessing(void);

    virtual uint32_t GetFC(void);
    virtual uint32_t GetFS(void);
    virtual uint32_t SetFC(uint32_t fc);
    virtual uint32_t SetFS(uint32_t fs);

    virtual bool FromFile(void);
    virtual bool FromDongle(void);

    virtual void ReadAsync(void *data_needed);
    virtual bool EverythingOK(void);

    virtual void HandleDrifts(float fc_drift, float fs_drift);

    /**
     *   Chose integer:
      SRC_SINC_BEST_QUALITY       > 0,
      SRC_SINC_MEDIUM_QUALITY     > 1,
      SRC_SINC_FASTEST            > 2,
      SRC_ZERO_ORDER_HOLD         > 3,
      SRC_LINEAR                  > 4
     */
    void ResamplerType(int quality);

#ifndef GOOGLE_UNIT_TEST
private:
#endif
    int file_descriptor;
    uint32_t s_rate,c_freq; ///< these values need to be kept
    unsigned char *file_wrapper_buffer;
    ResamplingRingBuffer* res_buffer;

};

#endif /* FILEDATAFEEDER_H_ */
