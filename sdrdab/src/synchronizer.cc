/*
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

#include <algorithm>
#include <cmath>
#include <cstdio>
#include "synchronizer.h"


Synchronizer::Synchronizer(size_t size) :  null_position_(0), null_quality_(NULL_OK), fc_drift_(0), abs_sum_(NULL), sigPhaseRef_(NULL), mode_parameters_(NULL), fc_corr_(NULL), fc_search_range_(20){
	// allocation for DetectMode
	abs_data_ = new float[size];
	abs_run_sum_ = new float[size];
}

Synchronizer::Synchronizer(modeParameters *mode_parameters, size_t size) : FFTEngine(mode_parameters->fft_size), null_position_(0), null_quality_(NULL_OK), fc_drift_(0), abs_run_sum_(NULL), mode_parameters_(mode_parameters), fc_corr_(NULL),fc_search_range_(20){

	// synchronization buffer size (max size)
	size_t synchro_size = mode_parameters_->frame_size;

	// allocation for DetectAndDecodeNULL
	abs_data_ = new float[synchro_size + mode_parameters_->null_size - 1];
	abs_sum_ = new float[synchro_size];

	// allocation for DetectPhaseReference
	sigPhaseRef_ = PhaseReferenceGen();
	fc_corr_ = new float[2 * mode_parameters_->fft_size];
}

void Synchronizer::Process(const float *data, size_t size, syncFeedback *out) {

	DetectAndDecodeNULL(data, size);

	// shift to the first frame sample (first PR sample)
	data += 2 * (null_position_ + mode_parameters_->null_size);
	size -= 2 * (null_position_ + mode_parameters_->null_size);

	// how should we set size of sigPR table
	DetectPhaseReference(data, size);

	out->null_position = null_position_;
	out->null_quality = null_quality_;
	out->fc_drift = fc_drift_;
}

Synchronizer::~Synchronizer() {
	delete[] abs_data_;
	delete[] abs_sum_;
	delete[] abs_run_sum_;

	delete [] sigPhaseRef_;
	delete [] fc_corr_;
}

void Synchronizer::DetectMode(const float *data, size_t size, syncDetect *out) {

	// compute abs of data
	for (size_t i = 0; i < size; i++) {
		abs_data_[i] = sqrt(*data * *data + *(data+1) * *(data+1));
		data += 2;
	}

	// compute abs run sum
	abs_run_sum_[0] = 0;
	for (size_t i = 0; i < size - 1; i++)
		abs_run_sum_[i+1] = 7.0/8.0 * abs_run_sum_[i] + abs_data_[i];

	// calculate null position of two frames
	size_t delay = 1e2;
	size_t skip = 2e4;

	size_t index = 0;
	size_t start = 0, end = 0;
	size_t null_counter = 0;

	for (size_t i = delay; i < size - 1; i++) {
		if (index < i) {
			if (abs_run_sum_[i] < abs_data_[i-delay]) {
				if (null_counter == 1) {
					start = i;
				} else if (null_counter == 2) {
					end = i;
					break;
				}
				null_counter++;
				index = i + skip;
			}
		}
	}

	out->null_position = start;

	// detect transmission mode
	size_t offset = 1e3;			// must be less than 24576
	size_t samples = 0;

	if (end > start)
		samples = end - start;

	fprintf(stderr, "first_null: %zu, second_null: %zu, frame_size: %zu\n", start, end, samples);

	if (samples > 196608-offset && samples < 196608+offset) {
		out->mode = DAB_MODE_I;
		return;
	}
	else if (samples > 98304-offset && samples < 98304+offset) {
		out->mode = DAB_MODE_IV;
		return;
	}
	else if (samples > 49152-offset && samples < 49152+offset) {
		float sum_m2_sub_m3 = 0;
		for (size_t i = end+365; i < end+644; i++)
			sum_m2_sub_m3 += abs_data_[i];

		float sum_m3 = 0;
		for (size_t i = end+20; i < end+325; i++)
			sum_m3 += abs_data_[i];

		float mean_m2_sub_m3 = sum_m2_sub_m3 / 279;
		float mean_m3 = sum_m3 / 305;
		//printf("\nmean_m2_sub_m3: %f, 3 * mean_m3: %f\n\n", mean_m2_sub_m3, 3 * mean_m3);
		if (mean_m2_sub_m3 > 3.0 * mean_m3) {
			out->mode = DAB_MODE_III;
			return;
		} else {
			out->mode = DAB_MODE_II;
			return;
		}
	}

	out->mode = DAB_MODE_UNKNOWN;
}

void Synchronizer::DetectAndDecodeNULL(const float* data, size_t size) {

	/*** detect null symbol ***/

	size_t synchro_size_short = mode_parameters_->null_size; /* 2*null_size */

	// compute abs of data
	for (size_t i = 0; i < synchro_size_short + mode_parameters_->null_size - 1; i++) {
		abs_data_[i] = fabs(*data) + fabs(*(data+1));
		data += 2;
	}

	// first element
	abs_sum_[0] = 0;
	for (size_t i = 0; i < mode_parameters_->null_size; i++)
		abs_sum_[0] += abs_data_[i];

	// remaining elements
	for (size_t i = 1; i < synchro_size_short; i++)
		abs_sum_[i] = abs_sum_[i-1] - abs_data_[i-1] + abs_data_[mode_parameters_->null_size+i-1];

	// position of minimum element
	size_t min_pos = 0;
	for (size_t i = 1; i < synchro_size_short; i++) {
		if (abs_sum_[i] < abs_sum_[min_pos])
			min_pos = i;
	}

	null_quality_ = NULL_OK;

	// min & max energy
	float min_energy = abs_sum_[min_pos];
	float max_energy = abs_sum_[0] + abs_sum_[synchro_size_short-1] - abs_sum_[min_pos];

	//printf("short -> null: %zu, min_energy: %f, max_energy: %f\n", min_pos, min_energy, max_energy);


	/*** if no detect null ***/

	int null_err = 50;

	if (abs(mode_parameters_->null_size/2 - min_pos) > null_err  || fabs(max_energy / min_energy) < 2) {

		size_t synchro_size_long = mode_parameters_->frame_size;

		// compute abs of data (only rest data)
		for (size_t i = synchro_size_short + mode_parameters_->null_size - 1; i < synchro_size_long + mode_parameters_->null_size - 1; i++) {
			abs_data_[i] = fabs(*data) + fabs(*(data+1));
			data += 2;
		}

		// remaining elements (only rest data)
		for (size_t i = synchro_size_short; i < synchro_size_long; i++)
			abs_sum_[i] = abs_sum_[i-1] - abs_data_[i-1] + abs_data_[mode_parameters_->null_size+i-1];

		// position of minimum element
		min_pos = 0;
		for (size_t i = 1; i < synchro_size_long; i++) {
			if (abs_sum_[i] < abs_sum_[min_pos])
				min_pos = i;
		}

		// min & max energy
		if (min_pos < synchro_size_long - mode_parameters_->null_size) {
			min_energy = abs_sum_[min_pos];
			max_energy = abs_sum_[min_pos+mode_parameters_->null_size];
		} else {
			min_energy = abs_sum_[min_pos];
			max_energy = abs_sum_[min_pos-mode_parameters_->null_size];
		}

		null_quality_ = NULL_SHIFT;

		//printf("long -> null: %zu, min_energy: %f, max_energy: %f\n", min_pos, min_energy, max_energy);

		// check if null is OK
		if (fabs(max_energy / min_energy) < 2) {
			min_pos = 0;
			null_quality_ = NULL_NOT_DETECTED;
			printf("\n******************* NULL not detected *************************\n\n");
		}
	}

	null_position_ = min_pos;
}


void Synchronizer::DetectPhaseReference(const float* data, size_t size) {
	size_t ncut = 25;	// TZ!!! assumed max offset error +/-
	size_t fft_size = mode_parameters_->fft_size;
	size_t guard_size = mode_parameters_->guard_size;
	float real_z = 0, imag_z = 0;
	const float* wdata = data;

	/*** calculate df_fract ***/
	for (size_t i = 2*ncut; i<2*guard_size-2*ncut; i += 2) {
		real_z += wdata[i] * wdata[i+2*fft_size]
				+ wdata[i+1] * wdata[i+2*fft_size+1];

		imag_z += wdata[i] * wdata[i+2*fft_size+1]
				- wdata[i+1] * wdata[+i+2*fft_size];
	}
	float fc_fract = atan2(imag_z, real_z)/(2*M_PI);

	/*** calculate df_int ***/
	for (size_t i = 0; i<2*fft_size; i+=2){
		// real
		fc_corr_[i] = data[2*guard_size+i] * sigPhaseRef_[2*guard_size+i]
							   + data[2*guard_size+i+1] * sigPhaseRef_[2*guard_size+i+1];
		// imag
		fc_corr_[i+1] = data[2*guard_size+i+1] * sigPhaseRef_[2*guard_size+i-1+1]
								  -data[2*guard_size+i-1+1] * sigPhaseRef_[2*guard_size+i+1];
	}
	FFT(fc_corr_);

	int fc_int=0;
	float fc_val=fc_corr_[0]*fc_corr_[0]+fc_corr_[1]*fc_corr_[1];			// zero shift (df_int=0) value, power instead of abs
	for (size_t i=2; i<2*fc_search_range_+2; i+=2){							// positive shift
		float val = fc_corr_[i]*fc_corr_[i]+fc_corr_[i+1]*fc_corr_[i+1];
		if(val>fc_val){
			fc_val=val;
			fc_int=i/2;
		}
	}
	for (size_t i=2*fft_size-2*fc_search_range_; i<2*fft_size; i+=2){		// negative shift
		float val = fc_corr_[i]*fc_corr_[i]+fc_corr_[i+1]*fc_corr_[i+1];
		if(val>fc_val){
			fc_val=val;
			fc_int=-(2*fft_size-i)/2;
		}
	}

	fc_drift_ = fc_int + fc_fract;
}

float* Synchronizer::PhaseReferenceGen(){

	//Generation of Phase Reference Symbol in Time & Freq, ETSI pp. 147-149

	size_t size = mode_parameters_->number_of_carriers + 1;
	size_t ncarriers = mode_parameters_->number_of_carriers / 2;

	int **phase_ref_index_tab = new int*[mode_parameters_->number_cu_per_symbol];

	if (mode_parameters_->dab_mode == DAB_MODE_I){

		// Table 39, page 148, only for mode=1
		//     				  				  kmin  kmax    k'  i  n
		int phase_ref_index_handle[][5] = { { -768, -737, -768, 0, 1 },
											{ -736, -705, -736, 1, 2 },
											{ -704, -673, -704, 2, 0 },
											{ -672, -641, -672, 3, 1 },
											{ -640, -609, -640, 0, 3 },
											{ -608, -577, -608, 1, 2 },
											{ -576, -545, -576, 2, 2 },
											{ -544, -513, -544, 3, 3 },
											{ -512, -481, -512, 0, 2 },
											{ -480, -449, -480, 1, 1 },
											{ -448, -417, -448, 2, 2 },
											{ -416, -385, -416, 3, 3 },
											{ -384, -353, -384, 0, 1 },
											{ -352, -321, -352, 1, 2 },
											{ -320, -289, -320, 2, 3 },
											{ -288, -257, -288, 3, 3 },
											{ -256, -225, -256, 0, 2 },
											{ -224, -193, -224, 1, 2 },
											{ -192, -161, -192, 2, 2 },
											{ -160, -129, -160, 3, 1 },
											{ -128, -97, -128, 0, 1 },
											{ -96, 	-65, -96, 1, 3 },
											{ -64, 	-33, -64, 2, 1 },
											{ -32,	 0, -32, 3, 2 },
											{ 1, 32, 1, 0, 3 },
											{ 33, 64, 33, 3, 1 },
											{ 65, 96, 65, 2, 1 },
											{ 97, 128, 97, 1, 1 },
											{ 129, 160, 129, 0, 2 },
											{ 161, 192, 161, 3, 2 },
											{ 193, 224, 193, 2, 1 },
											{ 225, 256, 225, 1, 0 },
											{ 257, 288, 257, 0, 2 },
											{ 289, 320, 289, 3, 2 },
											{ 321, 352, 321, 2, 3 },
											{ 353, 384, 353, 1, 3 },
											{ 385, 416, 385, 0, 0 },
											{ 417, 448, 417, 3, 2 },
											{ 449, 480, 449, 2, 1 },
											{ 481, 512, 481, 1, 3 },
											{ 513, 544, 513, 0, 3 },
											{ 545, 576, 545, 3, 3 },
											{ 577, 608, 577, 2, 3 },
											{ 609, 640, 609, 1, 0 },
											{ 641, 672, 641, 0, 3 },
											{ 673, 704, 673, 3, 0 },
											{ 705, 736, 705, 2, 1 },
											{ 737, 768, 737, 1, 1 } };

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++)
			phase_ref_index_tab[i] = new int[5];

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++){
			for (size_t j = 0; j < 5; j++) {
				phase_ref_index_tab[i][j] = phase_ref_index_handle[i][j];
			}
		}
	}

	//%#############

	else if (mode_parameters_->dab_mode == DAB_MODE_II){
		//Table 40, page 148, only for mode_parameters_->dab_mode=2
		//     				  				  kmin  kmax    k'  i  n
		int phase_ref_index_handle[][5] = { { -192, -161, -192, 0, 2 },
											{ -160, -129, -160, 1, 3 },
											{ -128, -97, -128, 2, 2 },
											{ -96, -65, -96, 3, 2 },
											{ -64, -33, -64, 0, 1 },
											{ -32, 0, -32, 1, 2 },
											{ 1, 32, 1, 2, 0 },
											{ 33, 64, 33, 1, 2 },
											{ 65, 96, 65, 0, 2 },
											{ 97, 128, 97, 3, 1 },
											{ 129, 160, 129, 2, 0 },
											{ 161, 192, 161, 1, 3 } };

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++)
			phase_ref_index_tab[i] = new int[5];

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++){
			for (size_t j = 0; j < 5; j++) {
				phase_ref_index_tab[i][j] = phase_ref_index_handle[i][j];
			}
		}
	}

	//%#############

	else if (mode_parameters_->dab_mode == DAB_MODE_III){
		// Table 41, page 148, only for mode=3
		//    				 				 kmin  kmax  k'  i  n
		int phase_ref_index_handle[][5] = { { -96, -65, -96, 0, 2 },
											{ -64, -33, -64, 1, 3 },
											{ -32, 0, -32, 2, 0 },
											{ 1, 32, 1, 3, 2 },
											{ 33, 64, 33, 2, 2 },
											{ 65, 96, 65, 1, 2 } };

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++)
			phase_ref_index_tab[i] = new int[5];

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++){
			for (size_t j = 0; j < 5; j++) {
				phase_ref_index_tab[i][j] = phase_ref_index_handle[i][j];
			}
		}
	}

	//%#############

	else if (mode_parameters_->dab_mode == DAB_MODE_IV){
		// Table 42, page 149, only for mode=4
		//     				  				  kmin  kmax    k'  i  n
		int phase_ref_index_handle[][5] = { { -384, -353, -384, 0, 0 },
											{ -352, -321, -352, 1, 1 },
											{ -320, -289, -320, 2, 1 },
											{ -288, -257, -288, 3, 2 },
											{ -256, -225, -256, 0, 2 },
											{ -224, -193, -224, 1, 2 },
											{ -192, -161, -192, 2, 0 },
											{ -160, -129, -160, 3, 3 },
											{ -128, -97, -128, 0, 3 },
											{ -96, -65, -96, 1, 1 },
											{ -64, -33, -64, 2, 3 },
											{ -32, 0, -32, 3, 2 },
											{ 1, 32, 1, 0, 0 },
											{ 33, 64, 33, 3, 1 },
											{ 65, 96, 65, 2, 0 },
											{ 97, 128, 97, 1, 2 },
											{ 129, 160, 129, 0, 0 },
											{ 161, 192, 161, 3, 1 },
											{ 193, 224, 193, 2, 2 },
											{ 225, 256, 225, 1, 2 },
											{ 257, 288, 257, 0, 2 },
											{ 289, 320, 289, 3, 1 },
											{ 321, 352, 321, 2, 3 },
											{ 353, 384, 353, 1, 0 } };

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++)
			phase_ref_index_tab[i] = new int[5];

		for (size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++){
			for (size_t j = 0; j < 5; j++) {
				phase_ref_index_tab[i][j] = phase_ref_index_handle[i][j];
			}
		}
	}

	// Table 43, page 148
	// 			  j = 0  1  2  3  4  5  6  7  8  9................15 16...........................................31
	int h[][32] = { { 0, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 2, 2, 1, 1, 0, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 2, 2, 1, 1 },
					{ 0, 3, 2, 3, 0, 1, 3, 0, 2, 1, 2, 3, 2, 3, 3, 0, 0, 3, 2, 3, 0, 1, 3, 0, 2, 1, 2, 3, 2, 3, 3, 0 },
					{ 0, 0, 0, 2, 0, 2, 1, 3, 2, 2, 0, 2, 2, 0, 1, 3, 0, 0, 0, 2, 0, 2, 1, 3, 2, 2, 0, 2, 2, 0, 1, 3 },
					{ 0, 1, 2, 1, 0, 3, 3, 2, 2, 3, 2, 1, 2, 1, 3, 2, 0, 1, 2, 1, 0, 3, 3, 2, 2, 3, 2, 1, 2, 1, 3, 2 } };

	// Equation page 147: fi(k)=pi/2*(h(i+1,k-k'+1)+n) - i,k',n from table phase_ref_index_tab

	float * fi = new float[size];
	float * work = new float[2 * size];

	for (size_t k = 0; k < mode_parameters_->number_cu_per_symbol; k++){
		for (int kk = phase_ref_index_tab[k][0]; kk <= phase_ref_index_tab[k][1]; kk++){
			if (kk + ncarriers == ((size - 1) / 2)){
				fi[kk + ncarriers] = 0;
			}
			else {
				fi[kk + ncarriers] = (M_PI / 2) * (h[phase_ref_index_tab[k][3]][kk - phase_ref_index_tab[k][2]] + phase_ref_index_tab[k][4]);
			}
		}
	}

	for (size_t i = 0; i < 2 * size; i++){
		if (i == size){
			work[i] = 0;
		}
		else if (i % 2){
			work[i] = sin(fi[i/2]);
		}
		else {
			work[i] = cos(fi[i/2]);
		}
	}

	float * phase_ref_symb = new float[2*mode_parameters_->fft_size];
	for (size_t i = 0; i < 2 * mode_parameters_->fft_size; i++){
		phase_ref_symb[i] = 0;
	}

	for (size_t i = 0; i < 2 * (ncarriers + 1); i++){
		phase_ref_symb[i] = work[2 * ncarriers + i];
	}

	for (size_t i = 2 * mode_parameters_->fft_size - 1; i >= 2 * mode_parameters_->fft_size - 2 * ncarriers; i--){
		phase_ref_symb[i] = work[2 * ncarriers - 2 * mode_parameters_->fft_size + i];
	}

	phase_ref_symb[0] = 0;

	float * sig_phase_ref_symb = new float[2 * mode_parameters_->fft_size + 2 * mode_parameters_->guard_size];
	IFFT(phase_ref_symb);

	//Add cyclic prefix
	for (size_t i = 0; i < 2 * mode_parameters_->guard_size; i++){
		sig_phase_ref_symb[i] = phase_ref_symb[2 * mode_parameters_->fft_size - 2 * mode_parameters_->guard_size + i];
	}
	for (size_t i = 2 * mode_parameters_->guard_size; i < 2 * mode_parameters_->fft_size + 2 * mode_parameters_->guard_size; i++){
		sig_phase_ref_symb[i] = phase_ref_symb[i - 2 * mode_parameters_->guard_size];
	}

	//Deallocation
	for(size_t i = 0; i < mode_parameters_->number_cu_per_symbol; i++) {
		    delete [] phase_ref_index_tab[i];
	}
	delete [] phase_ref_index_tab;
	delete [] work;
	delete [] fi;
	delete [] phase_ref_symb;

	return sig_phase_ref_symb;
}
