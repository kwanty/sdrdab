/**
 * @class AbstractDataFeeder
 * @brief provides data to the system
 *
 * Abstract class to provide data.
 * Inherited by:
 * - DataFeeder (read data from RTL stick)
 * - FileDataFeeder (read data from file)
 *
 * @author Paweł Szulc <pawel_szulc@onet.pl>    - StartProcessing(), ReadAsync(), SetFC(), SetFS(), HandleDrifts()
 * @author Wojciech Szmyd <wojszmyd@gmail.com>  - Remodulate(), DCRemoval()
 * @author Kacper Patro patro.kacper@gmail.com
 * @date 7 July 2015
 * @version 1.0 beta
 * @copyright Copyright (c) 2015 Wojciech Szmyd, Kacper Patro
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

#ifndef ABSTRACT_DATA_FEEDER_H_
#define ABSTRACT_DATA_FEEDER_H_

//#define PI 3.141592653


#define DC_LENGTH 5

/// @cond
#include <cstddef>
#include <cstdlib>
#include <cstring>
/// @endcond
#include <cmath>
#include <complex>
#include <errno.h>
#include <stdexcept>
#include <signal.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtl-sdr.h>
#include <pthread.h>
#include <ctime>
#include "data_format.h"
#include "RingBuffer/resampling_ring_buffer.h"

/**
 * Start collecting data from DataFeeder
 * @param data_needed address to structure data_feeder_ctx_t (defined in "data_format.h")
 */
void *StartProcessing(void * data_needed);

class AbstractDataFeeder {
public:
	AbstractDataFeeder();

	virtual ~AbstractDataFeeder();

	/**
	 * Turn on printing of errors
	 */
	void VerbosityOn(void){ verbose = true;}
	/**
	 * Turn off printing of errors
	 */
	void VerbosityOff(void){ verbose = false;}
	/**
	 * Turn on debugging mode - will print partial results
	 */
	void DebugOn(void) { debug = true;}
	/**
	 * Turn off debugging mode
	 */
	void DebugOff(void){ debug = false;}

	/**
	 * Stop collecting data
	 */
	virtual void StopProcessing(void) = 0;

	/**
	 * enter asynchronous reading mode
	 * @param data_needed address of structure data_feeder_ctx_t passed literally from StartProcessing function
	 */
	virtual void ReadAsync(void *data_needed) = 0;

	/**
	 * get center frequency
	 * @return center frequency in Hertz
	 */
	virtual uint32_t GetFC(void) = 0;

	/**
	 * get sampling frequency
	 * @return sampling frequency in Hertz
	 */
	virtual uint32_t GetFS(void) = 0;

	/**
	 * set center frequency (doesn't do much for FileDataFeeder)
	 * @param fc chosen center frequency in Hertz
	 * @return center frequency set in Hertz
	 */
	virtual uint32_t SetFC(uint32_t fc) = 0;

	/**
	 * set sampling frequency
	 * @param fs chosen sampling frequency in Hertz
	 * @return sampling frequency set in Hertz
	 */

	virtual uint32_t SetFS(uint32_t fs) = 0;

	/**
	 * Checks if DataFeeder reads from file
	 */
	virtual bool FromFile(void) = 0;

	/**
	 * Checks if DataFeeder reads from USB stick
	 */
	virtual bool FromDongle(void) = 0;

	/**
	 * Method in which Synchronizer gives feedback about frequency drifts
	 * If only 1 parameter calculated, second one should  be 0
	 *
	 * @param fc_drift drift of carrier frequency in Hertz
	 * @param fs_drift drift of sampling frequency in ppm
	 *
	 */
	virtual void HandleDrifts(float fc_drift, float fs_drift) = 0;

	/*
	 * Method to check if DataFeeder has been initialized successfully (workaround runtime_error)
	 * @return true if everything OK, false otherwise :)
	 */
	bool IsRunning () { return running;}

	virtual bool EverythingOK (void) = 0;

	void	RemodulateOff (void) {
	  current_fc_offset = 0.0;
	  do_remodulate = false;
	}

	void	RemodulateOn (void) { do_remodulate = true;}

	void	DCRemovalOff (void) { do_dcremoval = false;}

	void	DCRemovalOn (void) {  do_dcremoval = true;}

	void	FSHandleOff (void) {
	  current_fs_offset = 0.0;
	  do_handle_fs = false;
	}

	void FSHandleOn (void) {  do_handle_fs = true;}

	void AGCOff (void) { do_agc = false;}

	void AGCOn (void) { do_agc = true;}

	int running;

	bool verbose, debug;

#ifndef GOOGLE_UNIT_TEST
protected:
#endif


  /**
   * Remodulate data ( e^j )
   * @param data beginning of data buffer: real,image,...,real,image
   * @param size number of samples to process (make it even, please)
   * @param frequencyShift frequency in Hz that fc have to be shifted, could be negative
   */
  void Remodulate(float *data, size_t size, float frequencyShift);

  /**
   * Removal of Direct Current
   * @param data beginning of data in rtl_samples_ (first element needs to be real)
   * @param size number of complex samples to process
   */

	float current_fs_offset;  // [Hz]
	float current_fc_offset;  // [Hz]
	float *write_previous;
//	float dc_memory[DC_LENGTH];
	RingBuffer<float> *real_dc_rb;
	RingBuffer<float> *imag_dc_rb;
//	int last_dc_iterator;
//	ResamplingRingBuffer *res_buffer;
	const static float pi = 3.141592653;
	bool do_remodulate, do_dcremoval, do_handle_fs, do_agc;

	// number and size of buffers used by dongle or filewrapper
	size_t inner_buf_num, inner_buff_size;


};

#endif /* ABSTRACT_DATA_FEEDER_H_ */
