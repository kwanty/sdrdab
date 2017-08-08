/**
 * @class Synchronizer
 * @brief All stuff related to synchronization.
 *
 * synchronization to NULL, PR, fc/fs, etc...
 *
 * @author Jaroslaw Bulat kwant@agh.edu.pl (Synchronizer)
 * @author Piotr Jaglarz pjaglarz@student.agh.edu.pl (Synchronizer::Process, Synchronizer::DetectMode, Synchronizer::DetectAndDecodeNULL)
 * @author Michal Rybczynski mryba@student.agh.edu.pl (Synchronizer::DetectPhaseReference, Synchronizer::PhaseReferenceGen)
 * @date 7 July 2015
 * @version 1.0 beta
 * @pre libfftw3
 *
 * @copyright Copyright (c) 2015 Jaroslaw Bulat, Piotr Jaglarz, Michal Rybczynski.
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


#ifndef SYNCHRONIZER_H_
#define SYNCHRONIZER_H_

/// @cond
#include <cstddef>
/// @endcond
#include "fft_engine.h"
#include "data_format.h"

class Synchronizer : FFTEngine{
public:
	/**
	 * First constructor for DetectMode
	 * @param size number of complex samples to process (DetectMode)
	 */
	Synchronizer(size_t size);

	/**
	 * Second constructor for others
	 * @param mode_parameters numerical parameters of currently used transmission mode
	 * @param size number of complex samples to process
	 */
	Synchronizer(modeParameters *mode_parameters, size_t size);

	/**
	 * Manage ,,logic'' of synchronization, deal with initialization
	 * (eg. in SYNC stage, NULL will be detected in the middle of the TR)
	 * provide fc_drift_ to DataFeeder{}
	 * @param data beginning of data in rtl_samples_ (DataFeeder{})
	 * @param size number of complex samples to process
	 * @param out structure with output data for Scheduler (null_pos + fc_drift)
	 */
	void Process(const float *data, size_t size, syncFeedback *out);							// process data
	virtual ~Synchronizer();

	/**
	 * Detect transmission Mode
	 * Both NULL should be ,,visible'' in data.
	 * @param data beginning of data in rtl_samples_ (DataFeeder{})
	 * @param size number of complex samples to process
	 * @param out structure with output data for Scheduler (mode + null_pos)
	 */
	void DetectMode(const float *data, size_t size, syncDetect *out);

#ifndef GOOGLE_UNIT_TEST
private:
#endif
	size_t null_position_;		///< position of first sample of NULL symbol related to beginning of the frame,
	nullQuality null_quality_;	///< null quality
	float fc_drift_;			///< carier frequency drift in Hz, 0 means no drift, could be negative
	float *abs_data_;			///< buffer for abs of data
	float *abs_sum_;			///< buffer of window sum
	float *abs_run_sum_;		///< buffer of run sum
	float *sigPhaseRef_; 		///< todo

	modeParameters *mode_parameters_; ///< numerical parameters of currently used transmission mode

	float *fc_corr_;					///< internal buffer for fc_drift calculation
	size_t fc_search_range_;	///< +- range for search integer fc_drift

	/**
	 * Detect position of NULL symbol, set null_position_
	 * Provide NULL position DataFeeder{}
	 * @param data beginning of data in rtl_samples_ (DataFeeder{})
	 * @param size number of complex samples to process
	 * @todo extract data from first NULL (identification of station)
	 */
	void DetectAndDecodeNULL(const float *data, size_t size);

	/**
	 * Detect position of Phase Reference and decode (FFT) first frame - it is reference to DQPSK
	 * extract fc drift from Phase Reference and set fc_drift_.
	 * @param data beginning of data in rtl_samples_ (DataFeeder{})
	 * @param size number of complex samples to process
	 * @todo at this point, length of frame should be depended only on transmission mode, change size to enum transmissionMode
	 */
	void DetectPhaseReference(const float* data, size_t size);
	
	/**
	 * Generate Phase Reference Symbol
	 * @return complex signal of Phase Reference Symbol
	 */
	float* PhaseReferenceGen();
};

#endif /* SYNCHRONIZER_H_ */
