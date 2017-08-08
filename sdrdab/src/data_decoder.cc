/*
 * @class DataDecoder
 * @brief Data decoding - from dqpsk to ADTS AAC cointainer
 *
 * Puncturer, Viterbi, CRC, ReedSoloon, etc... . Need to work with MPEG and AAC codec.
 * Decode FIC - provide data for audio channel extraction (from MSC) as well as side data from FIC
 * (eg. Electronic Program Guide)
 *
 * @author Jaroslaw Bulat kwant@agh.edu.pl (DataDecoder, DataDecoder::CRC16, DataDecoder::BinToDec, DataDecoder::SuperFrameHandle, DataDecoder::SuperframeCircshiftBuff, DataDecoder::FirecodeInit, DataDecoder::FirecodeCheck)
 * @author Dawid Rymarczyk rymarczykdawid@gmail.com (DataDecoder::ExtractDataFromFIC)
 * @author Jan Twardowski (DataDecoder::ExtractDataFromPacketMode)
 * @author Adrian Karbowiak karbowia@student.agh.edu.pl (DataDecoder::DataDecoder, (DataDecoder::Process, DataDecoder::CreateStation, DataDecoder::UpdateStation, DataDecoder::StationsStatus, DataDecoder::GetFicExtraInfo), (DataDecoder::EnergyDispersal, DataDecoder::EnergyDispGen))
 * @author Szymon Dabrowski szymon332@gmail.com (DataDecoder::FICDecoder, DataDecoder::MSCDecoder)
 * @author Ernest Biela ernest.biela@gmail.com (DataDecoder::TimeDeInterleaver)
 * @author Rafal Palej palej@student.agh.edu.pl (DataDecoder::DePuncturer, DataDecoder::DePuncturerFICInit, DataDecoder::DePuncturerMSCInit)
 * @author Michal Babiuch babiuch.michal@gmail.com (DataDecoder::ReedSolomonCorrection)
 * @author Tomasz Zieliński tzielin@agh.edu.pl, Jarosław Bułat kwant@agh.edu.pl (DataDecoder::DeViterbi, DataDecoder::DeViterbiInit)
 * @date 7 July 2015
 * @version 1.0 beta
 * @copyright Copyright (c) 2015 Jaroslaw Bulat, Dawid Rymarczyk, Jan Twardowski, Adrian Karbowiak, Szymon Dabrowski, Ernest Biela, Rafal Palej, Michal Babiuch, Tomasz Zieliński.
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

#include "data_decoder.h"
/// @cond
#include <cstdlib>
#include <cstring>
#include <stdint.h>
/// @endcond
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include "data_decoder_data.h"
extern "C" {
#include "ecc.h"
}



DataDecoder::DataDecoder(modeParameters *param) :
												mode_parameters(*param),
												energy_gen_data_fic(new uint8_t[param->number_of_fib_per_cif * 32]),
												energy_gen_data_msc(NULL),
												in_idx_tab(0),
												out_idx_tab(0),
												input_fic_decoder(NULL),
												depunctur_data_(new float[param->number_of_cif*param->number_samp_after_timedep]),
												binary_data_(new uint8_t [param->number_of_cif*param->number_samp_after_vit/8]),
												depunctur_data_msc(NULL),
												//								data_energ_chain(NULL),
												//								bits_out_msc(NULL),
												binary_data_msc(NULL),
												energ_disp_out(new uint8_t[param->number_of_cif*param->number_samp_after_vit/8]),
												super_frame_size_(0),
												superframe_capacity_(8),
												superframe_cifs_(0),
												superframe_(NULL),
												adts_head_idx_(0){
	/**
	 * Copy a content of the temporary devPI matrix to the relevant devPi matrix (for depuncturer)
	 */
	memcpy( devPI, tmpDevPI, sizeof(devPI));
	DePuncturerFICInit(); ///< init Depunctur_info data for FIC decoding

	/**
	 * Generate energy dispersal FIC xor vector
	 */
	EnergyDispersalGen(energy_gen_data_fic, mode_parameters.number_of_fib_per_cif * 256);

	/**
	 * Initialize Viterbi to proceed decoding
	 */
	DeViterbiInit();
}

DataDecoder::DataDecoder(stationInfo *info, modeParameters *param) :
														mode_parameters(*param),
														station_info(*info),
														energy_gen_data_fic(new uint8_t[param->number_of_fib_per_cif * 32]),
														energy_gen_data_msc(NULL),
														idx_tab_size(0),
														CU_size(64),
														in_idx_tab(0),
														out_idx_tab(0),
														audio_size(0),
														audio_and_pad_size(0),
														extra_pad_size(0),
														beg_pos(16),
														cur_pos(0),
														beg_fic_size(0),
														cur_fic_size(0),
														input_fic_decoder(NULL),
														depunctur_data_(new float[param->number_of_cif*param->number_samp_after_timedep]),
														binary_data_(new uint8_t [param->number_of_cif*param->number_samp_after_vit/8]),
														depunctur_data_msc(NULL),
														//										data_energ_chain(NULL),
														binary_data_msc(NULL),
														energ_disp_out(new uint8_t[param->number_of_cif*param->number_samp_after_vit/8]),
														super_frame_size_(0),
														superframe_capacity_(16),
														superframe_cifs_(0),
														superframe_(NULL),
														adts_head_idx_(0) {
	/**
	 * Zero MSCDecoder counters
	 */
	msc_info.cif_count = 0; ///< superFrame content
	msc_info.number_dab_frame = 0; ///< being incremented until appropriate number of Demodulator invokes is achieved
	msc_info.super_frame_sync = false; ///< superFrame content
	msc_info.adts_chanconf = 8;
	msc_info.adts_dacsbr = 1;

	/**
	 * Count padding sizes.
	 * They will be used by TimeDeinterleaver to obtain correct audio positions
	 * Also used by Depuncturer
	 */
	padding.leftPaddingOffset = (round(((static_cast<float>(station_info.sub_ch_start_addr) / static_cast<float>(mode_parameters.number_cu_per_symbol)) - floor(static_cast<float>(station_info.sub_ch_start_addr) / static_cast<float>(mode_parameters.number_cu_per_symbol))) * mode_parameters.number_cu_per_symbol)) * 64; ///> *CU, 1 CU = 64 float
	padding.rightPaddingOffset = (round((ceil((static_cast<float>(station_info.sub_ch_start_addr) + static_cast<float>(station_info.sub_ch_size)) / static_cast<float>(mode_parameters.number_cu_per_symbol)) - (static_cast<float>(station_info.sub_ch_start_addr) + static_cast<float>(station_info.sub_ch_size)) / static_cast<float>(mode_parameters.number_cu_per_symbol)) * mode_parameters.number_cu_per_symbol)) * 64; ///> * CU, 1 CU = 64 float
	padding.size = padding.leftPaddingOffset + padding.rightPaddingOffset;


	/*
	 * Copy a content of temporary devPI matrix to relevant devPi matrix (Depuncturer)
	 * Get full padding size from padding structure
	 */
	memcpy( devPI, tmpDevPI, sizeof(devPI));
	depunctur_info.padding_msc = padding.size;

	/*
	 * Count indispensable FICdecoder/MSCdecoder parameters (lrange, pi, after_depuncturer_total_len_. etc.)
	 */
	DePuncturerFICInit(); ///< case: FIC

	if(!station_info.IsLong) ///< case: MSC:
		DePuncturerMSCInit(station_info.audio_kbps, station_info.protection_level, station_info.IsLong, station_info.ProtectionLevelTypeB); ///< DAB mode version
	else
		DePuncturerMSCInit(station_info.sub_ch_size, station_info.protection_level, station_info.IsLong, station_info.ProtectionLevelTypeB); //< DAB+ mode version

	/**
	 *  MSCDecoder parameters
	 */

	msc_info.number_bits_per_cif = depunctur_info.after_depuncturer_total_len_msc/4 - 6;
	msc_info.number_bits_per_frame = mode_parameters.number_of_cif * msc_info.number_bits_per_cif;

	/*
	 * initialize MSCdecoder buffers with data from DePuncturerMSCInit
	 */
	depunctur_data_msc = new float[depunctur_info.after_depuncturer_total_len_msc*mode_parameters.number_of_cif];

	/**
	 * Generate energy dispersal FIC xor vector
	 */
	EnergyDispersalGen(energy_gen_data_fic, mode_parameters.number_of_fib_per_cif * 256);

	/**
	 * Generate energy dispersal MSC xor vector
	 */
	if((depunctur_info.after_depuncturer_total_len_msc-6) % 8){
		energy_gen_data_msc = new uint8_t[(depunctur_info.after_depuncturer_total_len_msc/4-6) / 8 + 1];
		binary_data_msc = new uint8_t[param->number_of_cif*(depunctur_info.after_depuncturer_total_len_msc/4-6)/ 8 + 1];
	}
	else{
		energy_gen_data_msc = new uint8_t[(depunctur_info.after_depuncturer_total_len_msc/4-6) / 8];
		binary_data_msc = new uint8_t[param->number_of_cif*(depunctur_info.after_depuncturer_total_len_msc/4-6) / 8];
	}
	EnergyDispersalGen(energy_gen_data_msc, depunctur_info.after_depuncturer_total_len_msc/4-6);

	/**
	 * Initialize Viterbi to proceed decoding
	 */
	DeViterbiInit();
	/**
	 * An auxiliary table of indices for TimeDeinterleaver
	 */
	audio_size = CU_size * info->sub_ch_size;
	audio_and_pad_size = audio_size + padding.size;
	idx_tab_size = audio_size * param->number_of_cif;
	beg_fic_size = (beg_pos / param->number_of_cif) * param->fic_size;
	in_idx_tab = new size_t[idx_tab_size];
	out_idx_tab = new size_t[idx_tab_size];
	for(size_t CIF_count = 0; CIF_count < param->number_of_cif; CIF_count++){
		extra_pad_size = CIF_count * padding.size;
		for (size_t i = CIF_count * audio_size; i < (CIF_count + 1) * audio_size; i++){
			cur_pos = pos_table[i%16];
			cur_fic_size = (cur_pos + CIF_count) / param->number_of_cif * param->fic_size;
			in_idx_tab[i]  = beg_pos * audio_and_pad_size + beg_fic_size + extra_pad_size + i;
			out_idx_tab[i] = cur_pos * audio_and_pad_size + cur_fic_size + extra_pad_size + i;
		}
	}

	// superframe
	superframe_ = new uint8_t[superframe_capacity_*msc_info.number_bits_per_cif/8];
	FirecodeInit();
}

void DataDecoder::Process(decodReadWrite* decod, std::list<stationInfo> * station_info_list, stationInfo *audioService, UserFICData_t *&user_fic_extra_data) {
	/**
	 *
	 * FIC decoding
	 * Main task: get the station_info_list for scheduler
	 *
	 */

	/**
	 * Decode FIC
	 */
	FICDecoder(decod->read_here);

	/*
	 * Create or update the stationInfo list. The SubChannel_Basic_Infrmation list is treated as an auxiliary list.
	 * A comparison loop is formed below. Following behaviour will be triggered:
	 * 1. If a station from the stationInfo list doesn't belong to the current SubChannel_Basic_Infrmation list there will be no updating.
	 * 2. If a station from the stationInfo list belongs to the current SubChannel_Basic_Infrmation list it will be updated.
	 * An Equivalent station from the SubChannel_Basic_Infrmation list will be removed from the SubChannel_Basic_Infrmation list.
	 * 3. If the SubChannel_Basic_Infrmation list isn't empty stations from it will be created.
	 */
	MCIdata.subChannel_Basic_Information.sort(SubChannel_Basic_Sort()); ///< sort by subchannel ids
	MCIdata.subChannel_Basic_Information.unique(SubChannel_Basic_Unique()); ///< find and delete all duplicates

	it_sil = station_info_list->begin(); ///< set the list pointer to the beginning
	while (it_sil != station_info_list->end()){
		UpdateStation(it_sil); ///< update given station
		++it_sil;
	}
	it_sbi = MCIdata.subChannel_Basic_Information.begin(); ///< set the list pointer to the beginning
	while (it_sbi != MCIdata.subChannel_Basic_Information.end()){
		CreateStation(station_info_list); ///< create given station
		++it_sbi;
	}

	/**
	 *
	 * MSCdecoding
	 * Main task: get the audio data and pass it to audiodecoder
	 *
	 */

	if (!(audioService->station_name.compare("nonexistent") == 0)){
		/**
		 * increment dab frame after demodulator
		 */
		if( msc_info.number_dab_frame * mode_parameters.number_of_cif <=  16) {
			msc_info.number_dab_frame++;
		}

		/**
		 * Decode MSC
		 */
		super_frame_size_ = 0;

		MSCDecoder(decod->read_here + mode_parameters.fic_size + padding.leftPaddingOffset,
				decod->read_size,
				decod->write_here,
				audioService->IsLong
		);

		decod->write_size = super_frame_size_;

		/*
		 * Renew FicExtraInfo structure.
		 * user_fic_extra_data must be freed by user application in
		 * Scheduler::ParametersFromSDR(UserFICData_t).
		 */
		user_fic_extra_data = new UserFICData_t();
		GetFicExtraInfo(user_fic_extra_data, audioService);
		user_fic_extra_data->stations = *station_info_list; //copy station info for user
	}
}

DataDecoder::~DataDecoder() {
	delete[] input_fic_decoder;
	delete[] depunctur_data_;
	delete[] binary_data_;
	delete[] energ_disp_out;
	delete[] energy_gen_data_fic;
	delete[] energy_gen_data_msc;
	delete[] in_idx_tab;
	delete[] out_idx_tab;
	delete[] depunctur_data_msc;
	delete[] binary_data_msc;
	delete[] superframe_;
}

void DataDecoder::FICDecoder(float *data){
	for(size_t i = 0; i < mode_parameters.number_of_cif; i++) {
		//--------De-puncturing--------
		DePuncturer(data+((mode_parameters.fic_size/mode_parameters.number_of_cif)*i), mode_parameters.fic_size/mode_parameters.number_of_cif, depunctur_data_+((depunctur_info.after_depuncturer_total_len_fic)*i), false, false);
		//---------Viterbi decoder--------------
		DeViterbi(depunctur_data_+depunctur_info.after_depuncturer_total_len_fic*i, depunctur_info.after_depuncturer_total_len_fic, binary_data_+((depunctur_info.after_depuncturer_total_len_fic/4-6)/8)*i);
	}

	//-----------Energy dispersal ------------------------------------------------------------------------
	EnergyDispersal(binary_data_, energy_gen_data_fic, energ_disp_out, mode_parameters.number_of_fib_per_cif * 256);

	//-----------CRC checking of FIBS --------------------------------------------------------------------
	uint8_t *p_energ;
	p_energ = energ_disp_out;
	size_t pos;
	uint8_t type;
	uint8_t length;
	Info_FIG.labels1.clear();
	Info_FIG.FIC_data_channel.clear();
	Info_FIG.labels2.clear();
	MCIdata.subChannel_Basic_Information.clear();
	MCIdata.basic_Service_And_Service_Component.clear();
	MCIdata.service_component_in_Packet_Mode.clear();
	MCIdata.service_component_with_Conditional_Access.clear();
	MCIdata.service_component_language.clear();
	MCIdata.service_linking_information.clear();
	MCIdata.service_component_global_definition.clear();
	MCIdata.region_definition.clear();
	MCIdata.user_application_information.clear();
	MCIdata.FEC_SubChannel_Organization.clear();
	MCIdata.programme_Number.clear();
	MCIdata.programme_Type.clear();
	MCIdata.announcement_support.clear();
	MCIdata.announcement_switching.clear();
	MCIdata.frequency_Information.clear();
	MCIdata.transmitter_Identification_Information.clear();
	MCIdata.OE_Services.clear();
	MCIdata.OE_Announcement_support.clear();
	MCIdata.OE_Announcement_switching.clear();
	MCIdata.FM_Announcement_support.clear();
	MCIdata.FM_Announcement_switching.clear();
	MCIdata.country_LTO_and_International_table.LTOstruct.clear();
	fic_data_exist_status.MCI_status = 0;
	fic_data_exist_status.extract_FIC_return = 32;
	fic_data_exist_status.labels1_status = 0;
	fic_data_exist_status.FIGtype_status = 0;

	for (size_t i = 0; i < mode_parameters.number_of_fib; i++)
	{
		if(CRC16(energ_disp_out, 32))
		{
			pos = 1;
			while(pos < 241)
			{
				if (static_cast<size_t>(*energ_disp_out) != 255)
				{
					type = (*energ_disp_out >> 5);
					length = (*energ_disp_out << 3);
					length = length >> 3;
					fic_data_exist_status.extract_FIC_return = ExtractDataFromFIC(energ_disp_out+1, static_cast<size_t>(length), type);
					StationsStatus(type, fic_data_exist_status.extract_FIC_return );
					energ_disp_out = energ_disp_out+static_cast<int>(length)+1;
					pos = pos + (static_cast<size_t>(length)+1)*8;
				}
				else
					break;
			}
			energ_disp_out = p_energ + (i+1) * 32;
		}
		else
			continue;
	}
	energ_disp_out = p_energ;
}

void DataDecoder::MSCDecoder(float *read_data, size_t read_size, uint8_t* write_data, bool is_dab){
	TimeDeInterleaver(read_data);

	if( msc_info.number_dab_frame * mode_parameters.number_of_cif >  16 )
	{
		for(size_t i = 0; i < mode_parameters.number_of_cif; i++) {
			DePuncturer(read_data+((depunctur_info.audiolen_msc/mode_parameters.number_of_cif)*i)+(depunctur_info.padding_msc*i),depunctur_info.audiolen_msc/mode_parameters.number_of_cif,depunctur_data_msc+(depunctur_info.after_depuncturer_total_len_msc)*i, true, !is_dab);

			//---------Viterbi decoder--------------
			DeViterbi(depunctur_data_msc+depunctur_info.after_depuncturer_total_len_msc*i, depunctur_info.after_depuncturer_total_len_msc, binary_data_msc+((depunctur_info.after_depuncturer_total_len_msc/4-6)/8)*i);
		}
		//------ calculate buffer for superframe --------------
		if(is_dab){
			size_t cifs_per_tr = mode_parameters.number_of_cif;
			size_t  bytes_per_cif = msc_info.number_bits_per_cif/8;
			uint8_t * superframe_write = superframe_+superframe_cifs_*bytes_per_cif;

			//-----------------Energy dispersal--------------------
			EnergyDispersal(binary_data_msc, energy_gen_data_msc, superframe_write, depunctur_info.after_depuncturer_total_len_msc/4 - 6);
			superframe_cifs_+= cifs_per_tr;

			SuperFrameHandle(superframe_, write_data);
		} else {
			EnergyDispersal(binary_data_msc, energy_gen_data_msc, write_data, depunctur_info.after_depuncturer_total_len_msc/4 - 6);
			super_frame_size_ = mode_parameters.number_of_cif * msc_info.number_bits_per_cif/8;

//			std::ofstream fs("out.mp2", std::ios::out | std::ios::binary | std::ios::app);
//			fs.write(reinterpret_cast<char*>(write_data), (super_frame_size_)*sizeof(uint8_t));
//			fs.close();
		}
	}
}


void DataDecoder::TimeDeInterleaver(float* data) {
	for (size_t i = 0; i < idx_tab_size; i++){
		data[out_idx_tab[i]] = data[in_idx_tab[i]];
	}
}


void DataDecoder::DePuncturerFICInit(){
	size_t nCif = mode_parameters.number_of_cif;
	if(mode_parameters.dab_mode != DAB_MODE_III){
		depunctur_info.lrange_fic[0] = 2016; 	depunctur_info.lpi_fic[0] = 16;
		depunctur_info.lrange_fic[1] = 276; 	depunctur_info.lpi_fic[1] = 15;
		depunctur_info.lrange_fic[2] = 12; 		depunctur_info.lpi_fic[2] = 8;
		depunctur_info.lrange_fic[3] = 0; 		depunctur_info.lpi_fic[3] = 0;
		depunctur_info.lrange_fic[4] = 0; 		depunctur_info.lpi_fic[4] = 0;
		depunctur_info.padding_fic = 0;
		depunctur_info.audiolen_fic = (depunctur_info.lrange_fic[0]+depunctur_info.lrange_fic[1]+depunctur_info.lrange_fic[2]) * nCif;
	}
	else
	{
		depunctur_info.lrange_fic[0] = 2784; 	depunctur_info.lpi_fic[0] = 16;
		depunctur_info.lrange_fic[1] = 276; 	depunctur_info.lpi_fic[1] = 15;
		depunctur_info.lrange_fic[2] = 12; 		depunctur_info.lpi_fic[2] = 8;
		depunctur_info.lrange_fic[3] = 0; 		depunctur_info.lpi_fic[3] = 0;
		depunctur_info.lrange_fic[4] = 0; 		depunctur_info.lpi_fic[4] = 0;
		depunctur_info.padding_fic = 0;
		depunctur_info.audiolen_fic = (depunctur_info.lrange_fic[0]+depunctur_info.lrange_fic[1]+depunctur_info.lrange_fic[2]) * nCif;
	}

	depunctur_info.after_depuncturer_total_len_fic = 0;
	for (size_t i = 0; i < 5; i++)
		depunctur_info.after_depuncturer_total_len_fic += ((depunctur_info.lrange_fic[i]) * 32) / (depunctur_info.lpi_fic[i]+8);
}


void DataDecoder::DePuncturerMSCInit(size_t kbps_sbchsize, uint8_t protection, bool uep, bool isBProtection){
	size_t nCif = mode_parameters.number_of_cif;
	if(!uep)
	{
		if(kbps_sbchsize == 32){
			size_t UEPTAB[5][9] = {{3, 4, 17, 0, 5,  3,  2,  0,  0},
					{3, 3, 18, 0, 11, 6,  5,  0,  0},
					{3, 4, 14, 3, 15, 9,  6,  8,  0},
					{3, 4, 14, 3, 22, 13, 8,  13, 0},
					{3, 5, 13, 3, 24, 17, 12, 17, 4}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 48){
			size_t UEPTAB[5][9] = {{4, 3, 26, 3, 5, 4, 2, 3, 0},
					{3, 4, 26, 3, 9, 6, 4, 6, 0},
					{3, 4, 26, 3, 15, 10, 6, 9, 4},
					{3, 4, 26, 3, 24, 14, 8, 15, 0},
					{3, 5, 25, 3, 24, 18, 13, 18, 0}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 56){
			size_t UEPTAB[5][9] = {{6, 10, 23, 3, 5, 4, 2, 3, 0},
					{6, 10, 23, 3, 9, 6, 4, 5, 0},
					{6, 12, 21, 3, 16, 7, 6, 9, 0},
					{6, 10, 23, 3, 23, 13, 8, 13, 8},
					{0, 0, 0, 0, 0, 0, 0, 0, 0}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 64){
			size_t UEPTAB[5][9] = {{6, 9, 31, 2, 5, 3, 2, 3, 0},
					{6, 9, 33, 0, 11, 6, 5, 0, 0},
					{6, 12, 27, 3, 16, 8, 6, 9, 0},
					{6, 10, 29, 3, 23, 13, 8, 13, 8},
					{6, 11, 28, 3, 24, 18, 12, 18, 4}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 80){
			size_t UEPTAB[5][9] = {{6, 10, 41, 3, 6, 3, 2, 3, 0},
					{6, 10, 41, 3, 11, 6, 5, 6, 0},
					{6, 11, 40, 3, 16, 8, 6, 7, 0},
					{6, 10, 41, 3, 23, 13, 8, 13, 8},
					{6, 10, 41, 3, 24, 17, 12, 18, 4}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 96){
			size_t UEPTAB[5][9] = {{7, 9, 53, 3, 5, 4, 2, 4, 0},
					{7, 10, 52, 3, 9, 6, 4, 6, 0},
					{6, 12, 51, 3, 16, 9, 6, 10, 4},
					{6, 10, 53, 3, 22, 12, 9, 12, 0},
					{6, 13, 50, 3, 24, 18, 13, 19, 0}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 112){
			size_t UEPTAB[5][9] = {{6, 13, 50, 3, 24, 18, 13, 19, 0},
					{14, 17, 50, 3, 5, 4, 2, 5, 0},
					{11, 21, 49, 3, 9, 6, 4, 8, 0},
					{11, 23, 47, 3, 16, 8, 6, 9, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 128){
			size_t UEPTAB[5][9] = {{12, 19, 62, 3, 5, 3, 2, 4, 0},
								   {11, 21, 61, 3, 11, 6, 5, 7, 0},
								   {11, 22, 60, 3, 16, 9, 6, 10, 4},
								   {11, 21, 61, 3, 22, 12, 9, 14, 0},
								   {11, 20, 62, 3, 24, 17, 13, 19, 8}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize ==  160){
			size_t UEPTAB[5][9] = {{11, 19, 87, 3, 5, 4, 2, 4, 0},
					{11, 23, 83, 3, 11, 6, 5, 9, 0},
					{11, 24, 82, 3, 16, 8, 6, 11, 0},
					{11, 21, 85, 3, 22, 11, 9, 13, 0},
					{11, 22, 84, 3, 24, 18, 12, 19, 0}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 192){
			size_t UEPTAB[5][9] = {{11, 20, 110, 3, 6, 4, 2, 5, 0},
					{11, 22, 108, 3, 10, 6, 4, 9, 0},
					{11, 24, 106, 3, 16, 10, 6, 11, 0},
					{11, 20, 110, 3, 22, 13, 9, 13, 8},
					{11, 21, 109, 3, 24, 20, 13, 24, 0}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 224){
			size_t UEPTAB[5][9] = {{12, 22, 131, 38, 8, 6, 2, 6, 4},
					{12, 26, 127, 3, 12, 8, 4, 11, 0},
					{11, 20, 134, 3, 16, 10, 7, 9, 0},
					{11, 22, 132, 3, 24, 16, 10, 5, 0},
					{11, 24, 130, 3, 24, 20, 12, 20, 4}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 256){
			size_t UEPTAB[5][9] = {{11, 24, 154, 3, 6, 5, 2, 5, 0},
					{11, 24, 154, 3, 12, 9, 5, 10, 4},
					{11, 27, 151, 3, 16, 10, 7, 10, 0},
					{11, 22, 156, 3, 24, 14, 10, 13, 8},
					{11, 26, 152, 3, 24, 19, 14, 18, 4}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 320){
			size_t UEPTAB[5][9] = {{11, 26, 200, 3, 8, 5, 2, 6, 4},
					{11, 25, 201, 3, 13, 9, 5, 10, 8},
					{0, 0, 0, 0, 0, 0, 0, 0, 0},
					{11, 26, 200, 3, 24, 17, 9, 17, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
		else if(kbps_sbchsize == 384){
			size_t UEPTAB[5][9] = {{11, 27, 247, 3, 8, 6, 2, 7, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0},
					{11, 24, 250, 3, 16, 9, 7, 10, 4},
					{0, 0, 0, 0, 0, 0, 0, 0, 0},
					{12, 28, 245, 3, 24, 20, 14, 23, 8}};

			depunctur_info.lpi_msc[0] = UEPTAB[5-protection][4];
			depunctur_info.lpi_msc[1] = UEPTAB[5-protection][5];
			depunctur_info.lpi_msc[2] = UEPTAB[5-protection][6];
			depunctur_info.lpi_msc[3] = UEPTAB[5-protection][7];
			depunctur_info.lpi_msc[4] = 8;

			depunctur_info.lrange_msc[0] = (UEPTAB[5-protection][0]*128*(depunctur_info.lpi_msc[0]+8))/32;
			depunctur_info.lrange_msc[1] = (UEPTAB[5-protection][1]*128*(depunctur_info.lpi_msc[1]+8))/32;
			depunctur_info.lrange_msc[2] = (UEPTAB[5-protection][2]*128*(depunctur_info.lpi_msc[2]+8))/32;
			depunctur_info.lrange_msc[3] = (UEPTAB[5-protection][3]*128*(depunctur_info.lpi_msc[3]+8))/32;
			depunctur_info.lrange_msc[4] = 12;

			depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
		}
	}
	else
	{
		size_t n = 0;
		size_t pi1 = 0;
		size_t pi2 = 0;
		size_t len1 = 0;
		size_t len2 = 0;

		if(isBProtection)
		{
			if(protection == 0)
			{
				n = kbps_sbchsize/27;
				pi1 = 10;
				pi2 = 9;
			}
			else if(protection == 1)
			{
				n = kbps_sbchsize/21;
				pi1 = 6;
				pi2 = 5;
			}
			else if(protection == 2)
			{
				n = kbps_sbchsize/18;
				pi1 = 4;
				pi2 = 3;
			}
			else if(protection == 3)
			{
				n = kbps_sbchsize/15;
				pi1 = 2;
				pi2 = 1;
			}
			station_info.audio_kbps = n * 32;
			len1 = (24*n) - 3;
			len2 = 3;
		}
		else
		{
			if(protection == 0)
			{
				n = kbps_sbchsize/12;
				pi1 = 24;
				pi2 = 23;
				len1 = (6*n) - 3;
				len2 = 3;
			}
			else if(protection == 1)
			{
				n = kbps_sbchsize/8;
				if (n == 1){
					pi1 = 13;
					pi2 = 12;
					len1 = 5;
					len2 = 1;
				}
				else {
					pi1 = 14;
					pi2 = 13;
					len1 = (2*n) - 3;
					len2 = (4*n) + 3;
				}
			}
			else if(protection == 2)
			{
				n = kbps_sbchsize/6;
				pi1 = 8;
				pi2 = 7;
				len1 = (6*n) - 3;
				len2 = 3;
			}
			else if(protection == 3)
			{
				n = kbps_sbchsize/4;
				pi1 = 3;
				pi2 = 2;
				len1 = (4*n) - 3;
				len2 = (2*n) + 3;
			}
			station_info.audio_kbps = n * 8;
		}

		depunctur_info.lpi_msc[0] = pi1;
		depunctur_info.lpi_msc[1] = pi2;
		depunctur_info.lpi_msc[2] = 8;
		depunctur_info.lpi_msc[3] = 0;
		depunctur_info.lpi_msc[4] = 0;

		depunctur_info.lrange_msc[0] = (len1*128*(pi1+8))/32;
		depunctur_info.lrange_msc[1] = (len2*128*(pi2+8))/32;
		depunctur_info.lrange_msc[2] = 12;
		depunctur_info.lrange_msc[3] = 0;
		depunctur_info.lrange_msc[4] = 0;

		depunctur_info.audiolen_msc = (depunctur_info.lrange_msc[0]+depunctur_info.lrange_msc[1]+depunctur_info.lrange_msc[2]+depunctur_info.lrange_msc[3]+depunctur_info.lrange_msc[4]) * nCif;
	}

	depunctur_info.after_depuncturer_total_len_msc = 0;
	for (size_t i = 0; i < 5; i++)
		depunctur_info.after_depuncturer_total_len_msc += ((depunctur_info.lrange_msc[i]) * 32) / (depunctur_info.lpi_msc[i]+8);
}


void DataDecoder::DePuncturer(const float* data, size_t datalen, float* depunctur, bool msc, bool uep) {
	//size_t nCifs = mode_parameters.number_of_cif;

	size_t norginals = 0;
	size_t blocks = 0;
	size_t devpi = 0;

	if (msc) {
		if (uep)
			blocks = 5;
		else
			blocks = 3;
	} else
		blocks = 3;

	for (size_t q = 0; q < blocks; q++) {
		if (msc){
			datalen = depunctur_info.lrange_msc[q];
			if (depunctur_info.lpi_msc[q] >= 24)
				devpi = 24;
			else
				devpi = depunctur_info.lpi_msc[q];
		} else {
			datalen = depunctur_info.lrange_fic[q];
			if (depunctur_info.lpi_fic[q] >= 24)
				devpi = 24;
			else
				devpi = depunctur_info.lpi_fic[q];
		}

		norginals = ((datalen) * 32) / (devpi+8);

		size_t v = devPI[devpi-1];
		size_t byte_offset = 0;
		for (size_t i = 0; i < norginals; i++) {
			*depunctur = 0;

			if (byte_offset == 32){
				byte_offset = 0;
			}

			if ((((v << byte_offset) >> 31) & 1) == 1) {
				*depunctur = *data;
				data++;
			}
			depunctur++;
			byte_offset++;
		}
	}
}


void DataDecoder::EnergyDispersal(uint8_t* data, uint8_t*energyGenData, uint8_t* output, size_t disp_gen_size){

	/**
	 * If size of disp_gen_size % 8 == 0, launch faster xor algorithm
	 */
	if(!(disp_gen_size % 8)){
		for (size_t i = 0; i < mode_parameters.number_of_cif; i++){ ///> mode dependent loop
			for (size_t k = 0; k < disp_gen_size/8; k++){
				*output = *data ^ *energyGenData; ///> XOR bytes
				output++;
				data++;
				energyGenData++; ///> next bytes
			}
			energyGenData -= disp_gen_size/8; ///> result
		}
	}

	/**
	 * universal disp_gen_size size xor algorithm (if size of disp_gen_size % 8 != 0)
	 */
	else{
		uint8_t energy_bit = 0; ///< 0 to 7 counter
		uint8_t data_bit = 0; ///< 0 to 7 counter

		for (size_t i = 0; i < mode_parameters.number_of_cif; i++){ ///> mode dependent loop
			for (size_t k = 0; k < disp_gen_size; k++){

				if(!data_bit){*output = 0;}

				if (((*data & (128 >> data_bit)) >> (7 - data_bit)) ^ ((*energyGenData & (128 >> energy_bit)) >> (7 - energy_bit))){
					*output |= 128 >> data_bit; ///> if (data bit xor energyGenData) == 1, bit in output = 1
				}

				data_bit++;
				energy_bit++;

				if (data_bit >= 8){ ///>next data/output byte
					data_bit = 0;
					output++;
					data++;
				}

				if (energy_bit >= 8) { ///> next energyGenData byte
					energy_bit = 0;
					energyGenData++;
				}

			}
			energy_bit = 0;
			energyGenData -= disp_gen_size/8; ///> reset pointer
		}
	}
}


void DataDecoder::EnergyDispersalGen(uint8_t* energy_dispersal_table, size_t disp_gen_size) {
	/**
	 * EnergyDispersal sequence generation
	 * BYTEwise. Disp gen size in bits!
	 */
	size_t vector = 511; ///> 511 = 00000000 00000000 00000001 11111111 in bits in size t

	for (size_t i = 0; i < (disp_gen_size/8); i++){
		*energy_dispersal_table = 0;
		for (size_t k = 0; k < 8; k++){ ///> fill byte if 1
			if (((vector & 16) >> 4) ^ (vector & 1)){ ///> (vector AND ...00010000) XOR (vector AND 00000001). Only check
				(vector >>= 1) |= 256; ///> shift vector right by 1 and OR with ...100000000. Permanently
				*energy_dispersal_table |= 128 >> k; ///> OR energyGenData with a bit: (10000000 >> k). Permanently.
			}
			else{
				vector >>= 1; ///>shift vector right by 1 if 0. Permanently
			}
		}
		energy_dispersal_table++; ///> next byte
	}

	if (disp_gen_size % 8){
		*energy_dispersal_table = 0;
		for (size_t k = 0; k < (disp_gen_size % 8); k++){ ///> fill byte if 1
			if (((vector & 16) >> 4) ^ (vector & 1)){ ///> (vector AND ...00010000) XOR (vector AND 00000001). Only check
				(vector >>= 1) |= 256; ///> shift vector right by 1 and OR with ...100000000. Permanently
				*energy_dispersal_table |= 128 >> k; ///> OR energyGenData with a bit: (10000000 >> k). Permanently.
			}
			else{
				vector >>= 1; ///>shift vector right by 1 if 0. Permanently
			}
		}
	}
}


/// @todo: refractoring: write bits directly to output, without via variables
/// @todo: implement firecode error correction
void DataDecoder::SuperFrameHandle(uint8_t *data, uint8_t *write_data){
	size_t bytes_per_cif = msc_info.number_bits_per_cif/8;
	super_frame_size_ = 0;
	bool superframe = false;
	for( size_t idx=adts_head_idx_; idx<superframe_cifs_; ++idx){
		if( FirecodeCheck(data+idx*bytes_per_cif) ){		// found/verify synchronization!
			adts_head_idx_ = idx;
			if( superframe_cifs_-adts_head_idx_ >= 5 ){
				superframe = true;
				break;
			}else{											// not enough data for decoding adts (need 5 CIFs) or header not found
				SuperframeCircshiftBuff(data);
				return;
			}
		}
	}

	if(!superframe){										// not found superframe
		SuperframeCircshiftBuff(data);
		return;
	}

	uint8_t *adts_frame = data + adts_head_idx_*bytes_per_cif;
	ReedSolomonCorrection(adts_frame, 5*bytes_per_cif);
	///@todo how many errors should be trigger for 120/110 RS?

	// decoding ADTS starts here
	uint8_t dac_sbr=(adts_frame[2]&0x60)>>5;	// (18-19) dec_rate & sbr_flag at once
	uint8_t num_aus=0;
	uint8_t	adts_dacsbr=0;						// use last 4 bits
	uint8_t adts_chanconf=0;					// last 3 bits for channel index
	uint16_t au_start[7];						// worst case num_aus+1
	memset(au_start,0,7*sizeof(uint16_t));

	switch( dac_sbr ){
	case 0:    									// 00b, dac=0, sbr=0
		num_aus = 4;                            // number of Access Units (AUs)
		au_start[0] = 8;                        // address of the first AU
		adts_dacsbr = 0x05;          			// [0 1 0 1] freq index for ADTS header
		break;
	case 1:    									// 01b, dac=0, sbr=1
		num_aus = 2;                           	// number of AUs
		au_start[0] = 5;                        // address of the first AU
		adts_dacsbr = 0x08;               		// [1 0 0 0] freq index for ADTS header
		break;
	case 2:										// 10b, dac=1, sbr=0
		num_aus = 6;                            // number of AUs
		au_start[0] = 11;                       // address of the first AU
		adts_dacsbr = 0x03;              		// [0 0 1 1] freq index for ADTS header
		break;
	case 3:	    									// 11b, dac=1, sbr=1
		num_aus = 3;                          	// number of AUs
		au_start[0] = 6;                        // address of the first AU
		adts_dacsbr = 0x06;			         	// [0 1 1 0] freq index for ADTS header
		break;
	}

	//bool aac_channel_mode = (adts_frame[2]&0x10)>>3;	// (20-bit) 0=mono, 1=stereo
	if(adts_frame[2]&0x08){								// (21-bit) parametric stereo (PS)
		adts_chanconf = 0x2;							// [ 0 1 0 ] channel index for ADTS header
	} else {
		adts_chanconf = 0x1; 							// [ 0 0 1 ] channel index for ADTS header
	}
	//uint8_t mpeg_surround_config = adts_frame[2]&0x07;

	for( size_t r=1; r<num_aus; ++r) 								// addresses of the AUs from 2:last
		au_start[r] = BinToDec( adts_frame, 24+12*(r-1), 12 );  	// start from 25bit, each ADDR has 12bits
	au_start[num_aus] = station_info.audio_kbps/8*110;

	for( size_t r=0; r<num_aus; ++r)
		if(au_start[r]>=au_start[r+1]){
			adts_head_idx_ = 0;										// hard restart superframe handle
			superframe_cifs_ = 0;
			return;
		}

	size_t aac_size = 0;
	size_t crc_errors = 0;
	for( size_t r=0; r<num_aus; ++r ){
		if( !CRC16(adts_frame+au_start[r], au_start[r+1]-au_start[r]) ){
			crc_errors++;
			continue;
		}

		size_t au_size = au_start[r+1]-au_start[r]-2; 				// AU size in bytes, without CRC; -2 bytes?
		size_t adts_size = au_size + 7;                    			// frame size = au_size bytes of AU + 7 bytes of the adts-header

		uint8_t adts_header[7];
		memset( adts_header, 0, sizeof(uint8_t)*7 );
		adts_header[0] = 0xFF;					// (1:12) syncword
		adts_header[1] = 0xF0;
		adts_header[1] |= 0x08;            		// (13) 0=MPEG-4, 1=MPEG-2
		//adts_header[1] &= 0xF9;         		// (14-15) 00=MPEG-4, layer=MPEG-2
		adts_header[1] |= 0x01;           		// (16) 1 = no CRC, 0=CRC protection of adst_header is present (+2 bytes in the end)
		//adts_header[2] &= 0x3F;         		// (17:18) ? MPEG-4 audio object type minus 1
		adts_header[2] |= adts_dacsbr<<2;     	// (19:22) MPEG-4 sampling frequency index (15 not allowed); [ 0 1 1 0] ;
		//adts_header[2] &= 0xFD;           	// (23) 1=private stream
		adts_header[2] |= adts_chanconf>>2;   	// (24:26) MPEG-4 channel configuration; [ 0 0 1 ]
		adts_header[3] |= adts_chanconf<<6;   	// (24:26) MPEG-4 channel configuration; [ 0 0 1 ]
		//adts_header[3] &= 0xDF;               // (27) 1=originality
		//adts_header[3] &= 0xEF;               // (28) 1=home
		adts_header[3] |= 0x08;               	// (29) 1=copyright stream
		adts_header[3] |= 0x04;					// (30) 1=copywright start
		adts_header[3] |= adts_size>>11;   		// (31:43) frame length = 7 bytes of the adts header + au_size bytes of AU
		adts_header[4] = (adts_size>>3)&0xFF;
		adts_header[5] |= (adts_size&0x07)<<5;
		adts_header[5] |= 0x1F;      			// (44:54) unknown yet
		adts_header[6] |= 0xFC;
		//adts_header[6] &= 0x03;         		// (55:56) frames count in one packet

		memcpy( write_data+aac_size, adts_header, 7*sizeof(uint8_t) );
		aac_size +=7;
		memcpy( write_data+aac_size, adts_frame+au_start[r], ((au_start[r+1]-au_start[r])-2)*sizeof(uint8_t) );
		aac_size += ((au_start[r+1]-au_start[r])-2)*sizeof(uint8_t);
		super_frame_size_=aac_size;
	}

	// decoded, check next synchro
    if( adts_head_idx_+5 < superframe_cifs_ ){
    	if(crc_errors<num_aus)
    		adts_head_idx_ +=5;							// valid next superframe
    	else{
    		adts_head_idx_+=4;							// to many CRC erros, mayby synchronization glitch
    	}
    }else{												// next cif should be beginning of superframe
		adts_head_idx_ = 0;								// hard restart superframe handle
		superframe_cifs_ = 0;
		return;
    }
    SuperframeCircshiftBuff(data);
}


void DataDecoder::SuperframeCircshiftBuff(uint8_t *data){
	if( superframe_cifs_+mode_parameters.number_of_cif > superframe_capacity_ ){		// next cifs do not fit in buffer
		size_t bytes_per_cif = msc_info.number_bits_per_cif/8;

		if( (superframe_cifs_+mode_parameters.number_of_cif)-adts_head_idx_ > superframe_capacity_ ){ // lost synchro, buffer will be overfull
			superframe_cifs_ = 0;
		} else if( adts_head_idx_ < superframe_cifs_){
			memmove( data, data+adts_head_idx_*bytes_per_cif, (superframe_cifs_-adts_head_idx_)*bytes_per_cif );
			superframe_cifs_ -= adts_head_idx_;
		}else{																				// processed all cifs, next cif should be beginning of superframe, not tested!
			superframe_cifs_ = 0;
		}
		adts_head_idx_ = 0;
	}
}


uint16_t DataDecoder::BinToDec( uint8_t *data, size_t offset, size_t length ){
	uint32_t output = (*(data+offset/8)<<16) | ((*(data+offset/8+1)) << 8) | (*(data+offset/8+2));		// should be big/little endian save
	output >>= 24-length-offset%8;
	output &= (0xFFFF>>(16-length));
	return static_cast<uint16_t>(output);
}


bool DataDecoder::CRC16( uint8_t *data, size_t length ){
	uint16_t CRC = 0xFFFF;
	uint16_t poly= 0x1020;

	*(data+length-2) ^= 0xFF;
	*(data+length-1) ^= 0xFF;

	for( size_t i=0; i<length; ++i ){
		for( size_t b=0; b<8; ++b ){
			if( ((CRC&0x8000)>>15) ^ ((data[i]>>(7-b))&0x0001) ){
				CRC <<=1;
				CRC ^= poly;
				CRC |= 0x0001;
			} else {
				CRC <<=1;
				CRC &=0xFFFE;
			}
		}
	}

	*(data+length-2) ^= 0xFF;
	*(data+length-1) ^= 0xFF;

	if(CRC)
		return false;
	else
		return true;
}


/// Gnuradio + sdr-j implementation of firecode
///	g(x)=(x^11+1)(x^5+x^3+x^2+x+1)=1+x+x^2+x^3+x^5+x^11+x^12+x^13+x^14+x^16
void DataDecoder::FirecodeInit(void){
	uint8_t regs [16];
	uint16_t itab [8];

	for(size_t i=0; i<8; ++i){
		memset(regs, 0, 16*sizeof(uint8_t));
		regs[8+i] = 1;

		uint16_t z;
		for( size_t ii=0; ii<8; ++ii){
			z = regs[15];
			for(size_t jj=15; jj>0; --jj)
				regs[jj] = regs[jj-1] ^ (z & firecode_g[jj]);
			regs[0] = z;
		}

		uint16_t v = 0;
		for(int8_t ii=15; ii>=0; --ii)
			v = (v << 1) | regs[ii];
		itab[i] = v;

	}
	for(size_t i=0; i<256; ++i){
		firecode_tab [i] = 0;
		for(size_t j=0; j<8; ++j){
			if (i & (1 << j))
				firecode_tab [i] ^= itab[j];
		}
	}
}


bool DataDecoder::FirecodeCheck(const uint8_t *data){
	uint16_t state = (data[2]<<8) | data[3];
	uint16_t istate;

	for(size_t i=4; i<11; ++i){
		istate = firecode_tab[state >> 8];
		state = ((istate & 0x00ff) ^ data[i]) |	((istate ^ state << 8) & 0xff00);
	}

	for(size_t i=0; i<2; ++i){
		istate = firecode_tab[state >> 8];
		state = ((istate & 0x00ff) ^ data[i]) | ((istate ^ state << 8) & 0xff00);
	}

	if( !state )
		return true;
	else
		return false;
}


size_t DataDecoder::ReedSolomonCorrection(uint8_t *data, int32_t size) {
	int cell_size = 120;
	int no_cells = size/cell_size;					//14	=1680/120
	unsigned char temp_data[256];

	initialize_ecc ();
	int no_erasures = 0;
	int erasures_locations [0];
	size_t no_errors = 0;

	for (int i = 0; i < no_cells; i++){
		for (int j = 0; j < cell_size; j++) temp_data[j]=data[i+j*no_cells];
		decode_data(temp_data, cell_size);
		if (check_syndrome () != 0) {
			no_errors += static_cast<size_t>(correct_errors_erasures (temp_data, cell_size, no_erasures, erasures_locations));
			for (int j = 0; j < cell_size; j++) data[i+j*no_cells]=temp_data[j];
		}
	}
	return(no_errors);
}


uint8_t DataDecoder::ExtractDataFromFIC(uint8_t* data, size_t size, uint8_t FIG_Type){
	uint8_t FIG_extension;
	switch (FIG_Type)
	{
	case MCI:
	{

		FIG_extension = data[0] & 0x1F;
		switch (FIG_extension)
		{
		case ENSEMBLE:
		{
			struct MCI::Ensemble ensembledata;
			ensembledata.e_id = (static_cast<uint16_t> (data[1]) << 8) + data[2];
			ensembledata.change_flag = data[3] >> 6;
			uint8_t tmp = data[3] & 0xC0;
			ensembledata.al_flag = tmp >> 7;
			ensembledata.cif_count = (static_cast<uint16_t>(data[3] & 0x1F))
					+ data[4];
			if (ensembledata.change_flag == 1)
			{
				ensembledata.occurence_change = data[5];
			}
			else
			{
				ensembledata.occurence_change = 0;
			}
			MCIdata.ensemble = ensembledata;
		}
		break;

		case SUBCHANNEL_BASIC_INFORMATION:
		{
			struct MCI::SubChannel_Basic_Information kth_info;
			size_t number_of_bytes = 1;
			while (number_of_bytes < size){
				kth_info.subchannel_id = data[number_of_bytes] >> 2;
				kth_info.start_address =
						((static_cast<uint16_t>(data[number_of_bytes] & 0x03)) << 8)
						+ (static_cast<uint16_t>(data[number_of_bytes + 1]));
				kth_info.subchannel_size = 0;
				kth_info.protection_level = 0;
				kth_info.protection_level_typeB = 0;
				if (data[number_of_bytes + 2] >> 7)
				{ //Long form or not
					kth_info.is_long = 1;
					kth_info.subchannel_size =
							((static_cast<uint16_t>(data[number_of_bytes + 2] & 0x03)) << 2)
							+ (static_cast<uint16_t>(data[number_of_bytes + 3]));
					kth_info.protection_level =
							(data[number_of_bytes + 2] & 0x0C) >> 2;
					kth_info.protection_level_typeB =
							(data[number_of_bytes + 2] & 0x60) >> 4;
					kth_info.table6_subchannel_short_info = 0;
					number_of_bytes += 4;
					MCIdata.subChannel_Basic_Information.push_back(kth_info);
				}
				else
				{
					kth_info.table6_subchannel_short_info =
							data[number_of_bytes + 2] & 0x3F;
					kth_info.is_long = 0;
					number_of_bytes += 3;
					MCIdata.subChannel_Basic_Information.push_back(kth_info);
				}
			}

		}
		break;
		case BASIC_SERVICE_AND_SERVICE_COMPONENT:
		{
			struct MCI::Basic_Service_And_Service_Component bsasc;
			size_t number_of_byte = 1;
			bool tmpB = (data[0] & 0x20) >> 5;
			while (number_of_byte < size)
			{

				if (tmpB)
				{
					bsasc.service_id =
							((static_cast<int>(data[number_of_byte])) << 24)
							+ ((static_cast<int>(data[number_of_byte + 1])) << 16)
							+ ((static_cast<int>(data[number_of_byte + 2])) << 8)
							+ (static_cast<int>(data[number_of_byte + 3]));
					bsasc.LF= data[number_of_byte + 4] >> 7;
					bsasc.nbr_service_component = data[number_of_byte + 4] & 0x0F;
					number_of_byte += 5;
				}
				else
				{
					bsasc.service_id = ((static_cast<int>(data[number_of_byte])) << 8)
							+ data[number_of_byte + 1];
					bsasc.LF= data[number_of_byte + 2] >> 7;
					bsasc.nbr_service_component = data[number_of_byte + 2] & 0x0F;
					number_of_byte += 3;
				}
				for (int i = 0; i < bsasc.nbr_service_component; i++)
				{
					bsasc.transport_mech_id[i] = data[number_of_byte] >> 6;
					bsasc.PS[i] = (data[number_of_byte + 1] & 0x02) >> 1;
					bsasc.ca_flag[i] = data[number_of_byte + 1] & 0x01;
					switch (bsasc.transport_mech_id[i])
					{
					case 0:
					{
						bsasc.audio_service_component_type[i] = data[number_of_byte] & 0x3F;
						bsasc.subchannel_id[i] = data[number_of_byte + 1] >> 2;
						bsasc.data_service_component_type[i] = 0;
						bsasc.FIDC_id[i] = 0;
						bsasc.service_component_id[i] = 0;
					}
					break;
					case 1:
					{
						bsasc.data_service_component_type[i] = data[number_of_byte] & 0x3F;
						bsasc.subchannel_id[i] = data[number_of_byte + 1] >> 2;
					}
					break;
					case 2:
					{
						bsasc.data_service_component_type[i] = data[number_of_byte] & 0x3F;
						bsasc.FIDC_id[i] = data[number_of_byte + 1] >> 2;
					}
					break;
					case 3:
					{
						bsasc.service_component_id[i] =
								((static_cast<uint16_t>(data[number_of_byte] & 0x3F)) << 4)
								+ (data[number_of_byte + 1] >> 4);
					}
					break;
					}
					number_of_byte += 2;
				}
				MCIdata.basic_Service_And_Service_Component.push_back(bsasc);
			}

		}
		break;
		case SERVICE_COMPONENT_IN_PACKET_MODE:
		{
			struct MCI::Service_component_in_Packet_Mode scipm;
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				scipm.service_component_id = (static_cast<uint16_t>(data[number_of_byte] & 0x0F))
						+ (data[number_of_byte + 1] >> 4);
				scipm.CA_orga_flag = data[number_of_byte + 1] & 0x01;
				scipm.dGFlag = data[number_of_byte + 2] >> 7;
				scipm.rfu = (data[number_of_byte + 2] & 0x20) >> 5;
				scipm.dscty = data[number_of_byte + 2] & 0x3f;
				scipm.sub_channel_id = data[number_of_byte + 3] >> 2;
				scipm.packet_address =
						((static_cast<uint16_t>(data[number_of_byte + 3] & 0x03)) << 8)
						+ data[number_of_byte + 4];
				scipm.CAOrg =
						((static_cast<uint16_t>(data[number_of_byte + 4])) << 8)
						+ data[number_of_byte + 5];
				number_of_byte += 6;
				MCIdata.service_component_in_Packet_Mode.push_back(scipm);
			}
		}
		break;
		case SERVICE_COMPONENT_WITH_CONDITIONAL_ACCESS:
		{
			struct MCI::Service_component_with_Conditional_Access scwca;
			size_t number_of_byte = 1;
			while (number_of_byte < size){
				uint8_t tmp = data[number_of_byte] << 1;
				if (tmp >> 7)
				{
					scwca.sub_channel_id = data[number_of_byte] & 0x3F;
					scwca.f_id_cid = 0;
				}
				else
				{
					scwca.sub_channel_id = 0;
					scwca.f_id_cid = data[number_of_byte] & 0x3F;
				}
				scwca.CAOrg = (((uint16_t)(data[number_of_byte + 1])) << 8)
						+ data[number_of_byte + 2];
				number_of_byte += 3;
				MCIdata.service_component_with_Conditional_Access.push_back(scwca);
			}
		}
		break;
		case SERVICE_COMPONENT_LANGUAGE:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::Service_component_language scl;
				if (data[number_of_byte] >> 7)
				{//Long form
					scl.rfa = (data[number_of_byte] & 0x60) >> 5;
					scl.sc_id = ((static_cast<uint16_t>(data[number_of_byte] << 4)))
							+ data[number_of_byte + 1];
					scl.language = data[number_of_byte + 2];
					scl.f_id_cid = 0;
					scl.msc_id = 0;
					number_of_byte += 3;
				}
				else
				{
					if ((data[number_of_byte] & 0x40) >> 6)
					{
						scl.rfa = 0;
						scl.sc_id = 0;
						scl.f_id_cid = (data[number_of_byte] << 2) >> 6;
						scl.msc_id = 0;
					}
					else
					{
						scl.rfa = 0;
						scl.sc_id = 0;
						scl.msc_id = (data[number_of_byte] & 0x30) >> 4;
						scl.f_id_cid = 0;
					}

					scl.language = data[number_of_byte + 1];
					number_of_byte += 2;

				}
				MCIdata.service_component_language.push_back(scl);
			}
		}
		break;
		case SERVICE_LINKING_INFORMATION:
		{
			struct MCI::Service_linking_information sli;
			size_t number_of_byte = 1;
			while(number_of_byte < size){
				if (data[number_of_byte] >> 7)
				{
					sli.linkage_actuator = (data[number_of_byte] & 0x40);
					sli.soft_or_hard = (data[number_of_byte] & 0x20) >> 5;
					sli.international_linkage_indicator =
							(data[number_of_byte] & 0x10) >> 4;
					sli.linkage_set_number =
							(static_cast<uint16_t>(data[number_of_byte] & 0x0F) << 4)
							+ data[number_of_byte + 1];
					number_of_byte += 2;
				}
				else
				{
					sli.linkage_actuator = (data[number_of_byte] & 0x0F) >> 6;
					sli.soft_or_hard = (data[number_of_byte] & 0x20) >> 5;
					sli.international_linkage_indicator =
							(data[number_of_byte] & 0x10) >> 4;
					sli.linkage_set_number =
							(static_cast<uint16_t>(data[number_of_byte] & 0x0F) << 4)
							+ data[number_of_byte + 1];
					sli.number_of_ids = data[number_of_byte + 2] & 0x0F;
					if ((data[0] & 0x20) >> 5)
					{
						sli. rfu = data[number_of_byte + 2] >> 4;
						sli.id_list_qualifier = 0;
						sli.shorthand_indicator = 0;
						number_of_byte += 3;
						for (uint8_t i = 0; i < sli.number_of_ids; i++)
						{
							sli.s_id[i] = ((static_cast<int>(data[number_of_byte]) << 24))
									+ ((static_cast<int>(data[number_of_byte + 1])) << 16)
									+ ((static_cast<int>(data[number_of_byte + 2])) << 8)
									+ (static_cast<int>(data[number_of_byte + 3]));
							number_of_byte += 4;
							sli.ecc[i] = 0;
							sli.id[i] = 0;
						}

					}
					else
					{
						sli.rfu = data[number_of_byte + 2] >> 7;
						sli.id_list_qualifier = (data[number_of_byte + 2] & 0x60) >> 5;
						sli.shorthand_indicator = (data[number_of_byte + 2] & 0x10) >> 4;
						number_of_byte += 3;
						if (sli.international_linkage_indicator)
						{
							for (uint8_t i = 0; i < sli.number_of_ids; i++)
							{
								sli.ecc[i] = data[number_of_byte];
								sli.id[i] =
										((static_cast<uint16_t>(data[number_of_byte + 1])) << 8)
										+ data[number_of_byte + 2];
								sli.s_id[i] = 0;
								number_of_byte += 3;
							}
						}
						else
						{
							for (uint8_t i = 0; i < sli.number_of_ids; i++)
							{
								sli.ecc[i] = 0;
								sli.id[i] =
										((static_cast<uint16_t>(data[number_of_byte])) << 8)
										+ data[number_of_byte + 1];
								sli.s_id[i] = 0;
								number_of_byte += 2;
							}
						}
					}
				}
				MCIdata.service_linking_information.push_back(sli);
			}
		}
		break;
		case SERVICE_COMPONENT_GLOBAL_DEFINITION:
		{
			struct MCI::Service_component_global_definition scgd;
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				if ((data[0] & 0x20) >> 5)
				{
					scgd.service_id = ((static_cast<int>(data[number_of_byte])) << 24)
							+ ((static_cast<int>(data[number_of_byte + 1])) << 16)
							+ ((static_cast<int>(data[number_of_byte + 2])) << 8)
							+ data[number_of_byte + 3];
					scgd.sc_id_s = data[number_of_byte + 4] & 0x0F;
					if (data[number_of_byte + 5] >> 7)
					{ //Long
						scgd.service_component_id =
								(static_cast<uint16_t>(data[number_of_byte + 5] & 0x0F) << 8)
								+ data[number_of_byte + 6];
						if (data[number_of_byte + 4] >> 7)
						{
							scgd.rfa = data[number_of_byte + 7];
							number_of_byte += 8;
						}
						else
						{
							scgd.rfa = 0;
							number_of_byte += 7;
						}
						scgd.fidc_id = 0;
						scgd.subchannel_id = 0;
					}
					else
					{//Short
						scgd.service_component_id = 0;
						if ((data[number_of_byte + 5] & 0x40) >> 6)
						{
							scgd.fidc_id = data[number_of_byte + 5] & 0x3F;
							scgd.subchannel_id = 0;
						}
						else
						{
							scgd.fidc_id = 0;
							scgd.subchannel_id = data[number_of_byte + 5] & 0x3F;
						}
						if (data[number_of_byte + 4] >> 7)
						{
							scgd.rfa = data[number_of_byte + 6];
							number_of_byte += 7;
						}
						else
						{
							scgd.rfa = 0;
							number_of_byte += 6;
						}
					}
				}
				else
				{
					scgd.service_id = 0;
					scgd.service_id =
							((static_cast<int>(data[number_of_byte])) << 8)
							+ static_cast<int>(data[number_of_byte + 1]);
					scgd.sc_id_s = data[number_of_byte + 2] & 0x0F;
					if (data[number_of_byte + 3] >> 7)
					{ //Long
						scgd.service_component_id =
								(static_cast<int>(data[number_of_byte + 2] << 4) << 4)
								+ data[number_of_byte + 3];
						if (data[number_of_byte + 2] >> 7)
						{
							scgd.rfa = data[number_of_byte + 4];
							number_of_byte += 5;
						}
						else
						{
							scgd.rfa = 0;
							number_of_byte += 4;
						}
						scgd.fidc_id = 0;
						scgd.subchannel_id = 0;
					}
					else
					{//Short
						scgd.service_component_id = 0;
						if ((data[number_of_byte + 3] & 0x40) >> 6)
						{
							scgd.fidc_id = data[number_of_byte + 3] & 0x3F;
							scgd.subchannel_id = 0;
						}
						else
						{
							scgd.fidc_id = 0;
							scgd.subchannel_id = data[number_of_byte + 3] & 0x3F;
						}
						if (data[number_of_byte + 2] >> 7)
						{
							scgd.rfa = data[number_of_byte + 4];
							number_of_byte += 5;
						}
						else
						{
							scgd.rfa = 0;
							number_of_byte += 4;
						}
					}
				}
				MCIdata.service_component_global_definition.push_back(scgd);
			}

		}
		break;
		case COUNTRY_LTO_AND_INTERNATIONAL_TABLE:
		{
			MCIdata.country_LTO_and_International_table.lto_uniqe =
					(data[1] & 0x40) >> 6;
			MCIdata.country_LTO_and_International_table.ensemble_lto =
					data[1] & 0x3F;
			MCIdata.country_LTO_and_International_table.ensemble_ecc = data[2];
			MCIdata.country_LTO_and_International_table.international_table_id = data[3];
			if (data[0] >> 7)
			{
				size_t number_of_byte = 4;
				while(number_of_byte < size)
				{
					struct MCI::Country_LTO_and_International_table::LTO_IN_SERV lin;
					lin.number_of_services = data[number_of_byte] >> 6;
					lin.lto = data[number_of_byte] & 0x1F;
					if ((data[1] & 0x20) >> 5)
					{
						number_of_byte += 1;
						for(size_t i = 0; i < lin.number_of_services; i++)
						{
							lin.service_id_list[i] =
									((static_cast<int>(data[number_of_byte])) << 24)
									+ ((static_cast<int>(data[number_of_byte + 1])) << 16)
									+ ((static_cast<int>(data[number_of_byte + 2])) << 8)
									+ (static_cast<int>(data[number_of_byte + 3]));
							number_of_byte += 4;
						}
						lin.ecc = 0;

					}
					else
					{
						lin.ecc = data[number_of_byte + 1];
						number_of_byte += 2;
						for(size_t i = 0; i < lin.number_of_services; i++)
						{
							lin.service_id_list[i] =
									((static_cast<int>(data[number_of_byte])) << 8)
									+ (static_cast<int>(data[number_of_byte + 1]));
							number_of_byte += 2;
						}
					}
					MCIdata.country_LTO_and_International_table.LTOstruct.push_back(lin);
				}

			}
		}
		break;
		case DATE_AND_TIME:
		{
			MCIdata.date_and_time.rfu = data[1] >> 7;
			MCIdata.date_and_time.modified_julian =
					((static_cast<int>(data[1] & 0x7F)) << 10)
					+ ((static_cast<int>(data[2])) << 2)
					+ (static_cast<int>(data[3] >> 6));
			if ((data[3] & 0x10) >> 4)
			{
				MCIdata.date_and_time.hours = ((data[3] & 0x07) << 2) + (data[4] >> 6);
				MCIdata.date_and_time.minutes = (data[4] & 0x3F);
				MCIdata.date_and_time.seconds = (data[5] >> 2);
				MCIdata.date_and_time.miliseconds =
						((static_cast<uint16_t>(data[5] << 6) << 2))
						+ data[6];
			}
			else
			{
				MCIdata.date_and_time.hours = ((data[3] & 0x07) << 2) + (data[4] >> 6);
				MCIdata.date_and_time.minutes = data[4] & 0x3F;
				MCIdata.date_and_time.seconds = 0;
				MCIdata.date_and_time.miliseconds = 0;
			}
		}
		break;
		case REGION_DEFINITION:
		{
			size_t number_of_byte = 1;
			while(number_of_byte < size)
			{
				struct MCI::Region_definition rd;
				rd.geographical_area_type = data[number_of_byte] >> 4;
				rd.global_or_ensemble_flag = (data[number_of_byte] & 0x08) >> 7;
				rd.u_region_id = ((data[number_of_byte] & 0x07) << 3)
						+ (data[number_of_byte + 1] >> 6);
				rd.l_region_id = data[number_of_byte + 1] & 0x3F;
				if(rd.geographical_area_type)
				{
					rd.coordinates.latitude_coarse =
							((static_cast<int16_t>(data[number_of_byte + 2])) << 8)
							+ (static_cast<int16_t>(data[number_of_byte + 3]));
					rd.coordinates.longitude_coarse =
							((static_cast<int16_t>(data[number_of_byte + 4])) << 8)
							+ (static_cast<int16_t>(data[number_of_byte +5]));
					rd.coordinates.latitude_extent =
							((static_cast<uint16_t>(data[number_of_byte + 6])) << 4)
							+ (static_cast<uint16_t>(data[number_of_byte + 7] >> 4));
					rd.coordinates.longitude_extent =
							((static_cast<uint16_t>(data[number_of_byte + 7] & 0x0F)) << 8)
							+ (static_cast<uint16_t>(data[number_of_byte + 8]));
					number_of_byte += 9;
				}
				MCIdata.region_definition.push_back(rd);

			}
		}
		break;
		case USER_APPLICATION_INFORMATION:
		{ // (FIG 0 / 13)
			size_t number_of_byte = 1;
			while(number_of_byte < size)
			{
				struct MCI::User_application_information uai;
				if((data[0] & 0x20) >>5)
				{
					uai.service_id =
							((static_cast<int>(data[number_of_byte])) << 24)
							+ ((static_cast<int>(data[number_of_byte + 1])) << 16)
							+ ((static_cast<int>(data[number_of_byte+2])) << 8)
							+ ((static_cast<int>(data[number_of_byte+3])));
					uai.service_component_id = data[number_of_byte + 4] >> 4;
					uai.number_of_user_application = data[number_of_byte + 4] & 0x0F;
					uai.user_application_data_type =
							((static_cast<uint16_t>(data[number_of_byte + 5])) << 3)
							+ (data[number_of_byte + 6] >> 5);
					uai.user_application_data_length = data[number_of_byte + 6] & 0x1F;
					number_of_byte += 7;
				}
				else
				{
					uai.service_id = ((static_cast<int>(data[number_of_byte])) << 8)
							+ ((static_cast<int>(data[number_of_byte+1])));
					uai.service_component_id = data[number_of_byte + 2] >> 4;
					uai.number_of_user_application = data[number_of_byte + 2] & 0x0F;
					uai.user_application_data_type =
							((static_cast<uint16_t>(data[number_of_byte + 3])) << 3)
							+ (data[number_of_byte + 4] >> 5);
					uai.user_application_data_length = data[number_of_byte + 4] & 0x1F;
					number_of_byte +=5;
				}
				MCIdata.user_application_information.push_back(uai);
			}
		}
		break;
		case FEC_SUBCHANNEL_ORGANIZATION:
		{
			size_t number_of_bytes = 1;
			while (number_of_bytes < size){
				struct MCI::FEC_SubChannel_Organization fsco;
				fsco.subchannel_id = data[number_of_bytes] >> 2;
				fsco.FEC = data[number_of_bytes] & 0x01;
				number_of_bytes++;
				MCIdata.FEC_SubChannel_Organization.push_back(fsco);
			}
		}
		break;
		case PROGRAMME_NUMBER:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{

				struct MCI::Programme_Number pn;
				pn.service_id =
						((static_cast<uint16_t>(data[number_of_byte])) << 8)
						+ data[number_of_byte + 1];
				pn.programme_number =
						((static_cast<uint16_t>(data[number_of_byte + 2])) << 8)
						+ data[number_of_byte + 3];
				pn.continuation_flag = ((data[number_of_byte + 4] & 0x04) >> 2);
				pn.update_flag = ((data[number_of_byte + 4] & 0x02) >> 1);
				pn.new_service_id =
						((static_cast<uint16_t>(data[number_of_byte + 5])) << 8)
						+ data[number_of_byte + 6];
				pn.new_programme_id =
						((static_cast<uint16_t>(data[number_of_byte + 7])) << 8)
						+ data[number_of_byte + 8];
				number_of_byte += 9;
				MCIdata.programme_Number.push_back(pn);
			}
		}
		break;
		case PROGRAMME_TYPE:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::Programme_Type pt;
				pt.service_id =
						((static_cast<uint16_t>(data[number_of_byte])) << 8)
						+ data[number_of_byte + 1];
				pt.static_or_dynaminc = data[number_of_byte + 2] >> 7;
				pt.primary_or_secondary = (data[number_of_byte + 1] & 0x40) >> 6;
				if ((data[number_of_byte + 1] & 0x20) >> 5)
				{
					if ((data[number_of_byte + 1] & 0x10) >> 4)
					{
						pt.language = data[number_of_byte + 2];
						pt.international_code = data[number_of_byte + 3] & 0x1F;
						pt.complementary_code = data[number_of_byte + 4] & 0x1F;
						number_of_byte += 5;
					}
					else
					{
						pt.language = data[number_of_byte + 2];
						pt.international_code = data[number_of_byte + 3] & 0x1F;
						pt.complementary_code = 0;
						number_of_byte += 4;
					}
				}
				else
				{
					if ((data[number_of_byte + 1] & 0x10) >> 4)
					{
						pt.language = 0;
						pt.international_code = data[number_of_byte + 2] & 0x1F;
						pt.complementary_code = data[number_of_byte + 3] & 0x1F;
						number_of_byte += 4;
					}
					else
					{
						pt.language = 0;
						pt.international_code = data[number_of_byte + 2] & 0x1F;
						pt.complementary_code = 0;
						number_of_byte += 3;
					}
				}
				MCIdata.programme_Type.push_back(pt);
			}
		}
		break;
		case ANNOUCEMENT_SUPPORT:
		{
			size_t number_of_byte=1;
			while (number_of_byte < size)
			{
				struct MCI::Announcement_support as;
				as.service_id =
						((static_cast<uint16_t>(data[number_of_byte])) << 8)
						+ data[number_of_byte + 1];
				as.asu_flag =
						((static_cast<uint16_t>(data[number_of_byte + 2])) << 8)
						+ data[number_of_byte + 3];
				as.number_of_clusters = data[number_of_byte + 4] & 0x3F;
				for (uint8_t i = 0; i < as.number_of_clusters; i++)
				{
					as.clusters[i] = data[number_of_byte + 5 + i];
				}
				number_of_byte = number_of_byte + as.number_of_clusters + 5;
				MCIdata.announcement_support.push_back(as);
			}
		}
		break;
		case ANNOUCEMENT_SWITCHING:
		{
			size_t number_of_byte=1;
			while (number_of_byte < size)
			{
				struct MCI::Announcement_switching as;
				as.cluster_id = data[number_of_byte];
				as.asw_flags =
						((static_cast<uint16_t>(data[number_of_byte + 1])) << 8)
						+ data[number_of_byte + 2];
				as.new_flag = data[number_of_byte + 3] >> 7;
				if ((data[number_of_byte + 3] & 0x40) >> 6)
				{
					as.subchannel_id = data[number_of_byte + 3] & 0x1F;
					as.region_id = data[number_of_byte + 4] & 0x3F;
					number_of_byte += 5;
				}
				else
				{
					as.subchannel_id = data[number_of_byte + 3] & 0x1F;
					number_of_byte += 4;
				}
				MCIdata.announcement_switching.push_back(as);
			}
		}
		break;
		case FREQUENCY_INFORMATION:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::frequency_Information fi;
				fi.regionid =
						((static_cast<uint16_t>(data[number_of_byte])) << 3)
						+ (data[number_of_byte + 1] >> 5);
				fi.length_of_fr_list = data[number_of_byte + 1] & 0x1F;
				for (int i = 0; i < fi.length_of_fr_list; i++)
				{
					fi.id[i] = ((static_cast<uint16_t>(data[number_of_byte + 2])) << 8)
							+ data[number_of_byte + 3];
					fi.RM[i] = data[number_of_byte + 4] >> 4;
					fi.continuity_flag[i] =
							(data[number_of_byte + 4] & 0x0F) >> 3;
					fi.length_of_freq_list[i] = data[number_of_byte+4] & 0x07;
					switch (fi.RM[i])
					{
					case 0:
					case 1:
					{
						number_of_byte += 5;
						for (int j = 0; j < fi.length_of_freq_list[i]; j++){
							fi.control_field[i][j] = data[number_of_byte] >> 3;
							fi.frequency[i][j] =
									((static_cast<int>(data[number_of_byte] & 0x07)) << 16)
									+ ((static_cast<int>(data[number_of_byte + 1])) << 8)
									+ (static_cast<uint16_t>(data[number_of_byte+2]));
							number_of_byte += 3;
						}

					}
					break;
					case 8:
					case 9:
					case 10:
					{
						number_of_byte += 5;
						for (int j = 0; j < fi.length_of_freq_list[i]; j++){
							fi.frequency[i][j] = data[number_of_byte];
							fi.control_field[i][j] = 0;
							number_of_byte ++;
						}

					}
					break;
					case 12:
					{
						number_of_byte += 5;
						for (int j = 0; j < fi.length_of_freq_list[i]; j++){
							fi.frequency[i][j] =
									((static_cast<int>(data[number_of_byte])) << 8)
									+ data[number_of_byte + 1];
							fi.control_field[i][j] = 0;
							number_of_byte+=2;
						}

					}
					break;
					case 14:
					case 6:
					{
						number_of_byte += 5;
						fi.id_field[i] = data[number_of_byte + 5];
						number_of_byte += 6;
						for (int j = 0; j < fi.length_of_freq_list[i]; j = j + 3){
							fi.frequency[i][j] =
									((static_cast<int>(data[number_of_byte])) << 8)
									+ (static_cast<int>(data[number_of_byte]));
							fi.control_field[i][j] = 0;
							number_of_byte += 3;
						}

					}
					break;
					default:
						break;
					}
				}
				MCIdata.frequency_Information.push_back(fi);
			}
		}
		break;
		case TRANSMITTER_IDENTIFICATION_IINFORMATION:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::Transmitter_Identification_Information ttid;
				if (data[number_of_byte >> 7])
				{
					ttid.main_id = data[number_of_byte] & 0x7F;
					ttid.number_of_sub_fields = data[number_of_byte + 1] & 0x07;
					number_of_byte += 2;
					for (int i = 0; i < ttid.number_of_sub_fields; i++)
					{
						ttid.SubFields[i].sub_id = data[number_of_byte] >> 3;
						ttid.SubFields[i].td =
								(static_cast<uint16_t>(data[number_of_byte] & 0x07) << 8)
								+ data[number_of_byte + 1];
						ttid.SubFields[i].lattitude_offset =
								((static_cast<uint16_t>(data[number_of_byte + 2])) << 8)
								+ data[number_of_byte + 3];
						ttid.SubFields[i].longitude_offset =
								((static_cast<uint16_t>(data[number_of_byte + 4])) << 8)
								+ data[number_of_byte + 5];
						number_of_byte += 6;
					}
				}
				else
				{
					ttid.number_of_sub_fields = 0;
					ttid.main_id = data[number_of_byte] & 0x7F;
					ttid.lattitude_coarse =
							((static_cast<uint16_t>(data[number_of_byte + 1])) << 8)
							+ data[number_of_byte + 2];
					ttid.longitude_coarse =
							((static_cast<uint16_t>(data[number_of_byte + 3])) << 8)
							+ data[number_of_byte + 4];
					ttid.lattitude_fine = data[number_of_byte + 5] >> 4;
					ttid.longitude_fine = data[number_of_byte + 5] & 0x0F;
					number_of_byte += 6;
				}
				MCIdata.transmitter_Identification_Information.push_back(ttid);
			}
		}
		break;
		case OE_SERVICES:
		{
			size_t number_of_byte = 1;
			while(number_of_byte<size)
			{
				struct MCI::OE_Services os;
				if((data[0] & 0x20) >> 5)
				{
					os.service_id =
							((static_cast<int>(data[number_of_byte]) << 24))
							+ ((static_cast<int>(data[number_of_byte + 1])) << 16)
							+ ((static_cast<int>(data[number_of_byte + 2])) << 8)
							+ (static_cast<int>(data[number_of_byte + 3]));
					os.conditional_access_id = (data[number_of_byte + 4] & 0x30) >> 4;
					os.number_of_eids = data[number_of_byte + 4] >> 4;
					number_of_byte += 5;
					for (uint8_t i=0; i<os.number_of_eids; i++ )
					{
						os.ensemble_id[i] =
								((static_cast<uint16_t>(data[number_of_byte])) << 8)
								+ data[number_of_byte + 1];
						number_of_byte += 2;
					}
				}
				else
				{
					os.service_id =
							((static_cast<uint16_t>(data[number_of_byte])) << 8)
							+ (static_cast<uint16_t>(data[number_of_byte + 1]));
					os.conditional_access_id = (data[number_of_byte + 2] & 0x30) >> 4;
					os.number_of_eids = data[number_of_byte + 2] >> 4;
					number_of_byte += 3;
					for (uint8_t i=0; i<os.number_of_eids; i++ )
					{
						os.ensemble_id[i] =
								((static_cast<uint16_t>(data[number_of_byte])) << 8)
								+ data[number_of_byte+1];
						number_of_byte += 2;
					}
				}
				MCIdata.OE_Services.push_back(os);
			}
		}
		break;
		case OE_ANNOUCEMENT_SUPPORT:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::OE_Announcement_support oas;
				oas.service_id =
						((static_cast<uint16_t>(data[number_of_byte])) << 8)
						+ data[number_of_byte + 1];
				oas.asu_flag =
						((static_cast<uint16_t>(data[number_of_byte + 2])) << 8)
						+ data[number_of_byte + 3];
				oas.number_of_enseble_id = data[number_of_byte + 4] & 0x0F;
				number_of_byte += 5;
				for (int i = 0; i < oas.number_of_enseble_id; i++)
				{
					oas.ensembled_ids[i] =
							((static_cast<uint16_t>((data[number_of_byte]))) << 8)
							+ data[number_of_byte + 1];
					number_of_byte += 2;
				}
				MCIdata.OE_Announcement_support.push_back(oas);
			}
		}
		break;
		case OE_ANNOUCEMENT_SWITCHING:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::OE_Announcement_switching oas;
				oas.cluster_id_current_ens = data[number_of_byte];
				oas.asw_flags =
						((static_cast<uint16_t>(data[number_of_byte + 1])) << 8)
						+ data[number_of_byte + 2];
				oas.new_flag = data[number_of_byte + 3] >> 7;
				if ((data[number_of_byte + 3] & 0x40) >> 6)
				{
					oas.region_id_current_ensemble = data[number_of_byte + 3] & 0x3F;
					oas.ensemble_id =
							((static_cast<uint16_t>(data[number_of_byte + 4])) << 8)
							+ data[number_of_byte + 5];
					oas.cluster_id_other_ens = data[number_of_byte + 6];
					oas.region_id = data[number_of_byte + 7] & 0x3F;
					number_of_byte += 8;
				}
				else
				{
					oas.region_id_current_ensemble = data[number_of_byte + 3] & 0x3F;
					oas.ensemble_id =
							((static_cast<uint16_t>(data[number_of_byte + 4])) << 8)
							+ data[number_of_byte + 5];
					oas.cluster_id_other_ens = data[number_of_byte + 6];
					oas.region_id = 0;
					number_of_byte += 7;
				}
				MCIdata.OE_Announcement_switching.push_back(oas);
			}
		}
		break;
		case FM_ANNOUCEMENT_SUPPORT:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::FM_Announcement_support fas;
				fas.service_id =
						((static_cast<uint16_t>(data[number_of_byte])) << 8)
						+ data[number_of_byte + 1];
				fas.number_of_pi_codes = data[number_of_byte + 2] & 0x0F;
				number_of_byte += 3;
				for (int i = 0; i < fas.number_of_pi_codes; i++)
				{
					fas.programme_ids[i] =
							((static_cast<uint16_t>((data[number_of_byte]))) << 8)
							+ data[number_of_byte + 1];
					number_of_byte += 2;
				}
				MCIdata.FM_Announcement_support.push_back(fas);
			}
		}
		break;
		case FM_ANNOUCEMENT_SWITCHING:
		{
			size_t number_of_byte = 1;
			while (number_of_byte < size)
			{
				struct MCI::FM_Announcement_switching fas;
				fas.cluster_id_current_ens = data[number_of_byte];
				fas.new_flag = data[number_of_byte + 1] >> 7;
				fas.region_id = data[number_of_byte + 1]  & 0x3F;
				fas.programme_id =
						((static_cast<uint16_t>(data[number_of_byte + 2])) << 8)
						+ data[number_of_byte + 3];
				number_of_byte += 4;
				MCIdata.FM_Announcement_switching.push_back(fas);
			}
		}
		break;
		case FIC_RE_DIRECTION:
		{
			MCIdata.FIC_re_direction.FIG0Flags =
					((static_cast<int>(data[1])) << 24)
					+ ((static_cast<int>(data[2])) << 16)
					+ (static_cast<int>(data[3]) << 8)
					+ (static_cast<int>(data[4]));
			MCIdata.FIC_re_direction.FIG1Flags = (data[5]);
			MCIdata.FIC_re_direction.FIG2Flags = (data[6]);
		}
		break;
		default:
			break;
		}
		break;
		case LABELS1:
		{
			struct InfoFIG::Labels1 l1;
			FIG_extension = data[0]  & 0x07;
			l1.charset = data[0] >> 4;
			l1.other_ensemble = (data[0] & 0x07) >> 3;
			l1.FM_Text_Label.id_field = 0;
			switch (FIG_extension)
			{
			case ENSEMBLE_LABEL:
			{
				l1.ensemble_Label.id_field =
						((static_cast<uint16_t>(data[1])) << 8) + data[2];
				l1.ensemble_Label.label.reserve(16+8);
				//16+8 should be sufficient, yet quick for most strings
				char buf[4];
				for(int i = 0; i < 16; i++){
					l1.ensemble_Label.label.append(
							EBULatinToUTF8(data[i+3], buf) );
				}
				l1.ensemble_Label.label.reserve(0); //free unused capacity
				Info_FIG.labels1.push_back(l1);
			}
			break;
			case FM_TEXT_LABEL:
			{
				l1.FM_Text_Label.id_field =
						((static_cast<uint16_t>(data[1])) << 8) + data[2];
				l1.FM_Text_Label.label.reserve(16+8);
				//16+8 should be sufficient, yet quick for most strings
				char buf[4];
				for(int i = 0; i < 16; i++){
					l1.FM_Text_Label.label.append(
							EBULatinToUTF8(data[i+3], buf));
				}
				l1.FM_Text_Label.label.reserve(0); //free unused capacity
				Info_FIG.labels1.push_back(l1);
			}
			break;
			default:
				break;
			}
		}
		break;
		case LABELS2:
		{
			struct InfoFIG::Labels2 l2;
			FIG_extension = data[0] & 0x07;
			l2.toggle_tag = data[0] >> 7;
			l2.segment_index = (data[0] & 0x70) >> 4;
			l2.OE = (data[0] & 0x08) >> 3;
			switch (FIG_extension)
			{
			case ENSEMBLE_LABEL:
			{
				l2.ensemble_Label.id_field =
						((static_cast<uint16_t>(data[1])) << 8) + data[2];
				l2.ensemble_Label.label.reserve(16);
				for(int i = 0; i < 16; i++){
					l2.ensemble_Label.label.push_back(data[i+3]);
				}
			}
			break;
			case FM_TEXT_LABEL:
			{
				l2.FM_Text_Label.id_field =
						((static_cast<uint16_t>(data[1])) << 8) + data[2];
				l2.FM_Text_Label.label.reserve(16);
				for(int i = 0; i < 16; i++)
					l2.FM_Text_Label.label.push_back(data[i+3]);

			}
			break;
			default:
				break;
				Info_FIG.labels2.push_back(l2);
			}
		}
		break;
		case FIC_DATA_CHANNEL:
		{
			struct InfoFIG::FIC_data_channel fdc;
			FIG_extension = data[0] & 0x07;
			fdc.D1 = data[0] >> 7;
			fdc.D2 = (data[0] & 0x40) >> 6;
			fdc.type_component_id = (data[0] & 0x30) >> 4;
			switch (FIG_extension)
			{
			case PAGING:
			{
				if (fdc.D1)
				{
					fdc.paging.sub_channle_id = data[1] >> 2;
					fdc.paging.packet_adress =
							((static_cast<uint16_t>(data[1] & 0x03)) << 8) + data[2];
					fdc.paging.F2 = (data[3] & 0x40) >> 6;
					fdc.paging.logical_frame_number =
							((static_cast<uint16_t>(data[3] & 0x1F)) << 8) + data[4];
					if (data[3] >> 7)
					{
						fdc.paging.F3 = data[5] >> 7;
						fdc.paging.time =
								((static_cast<uint16_t>(data[5] & 0x7F)) << 5)
								+ (data[6] >> 4);
						fdc.paging.ca_id = data[6] & 0x07;
						fdc.paging.ca_organization =
								((static_cast<uint16_t>(data[7])) << 8) + data[8];
						fdc.paging.paging_user_group =
								((static_cast<int>(data[9])) << 24)
								+ (static_cast<int>(data[10]) << 16)
								+ (static_cast<int>(data[11]) << 8)
								+ (static_cast<int>(data[12]));
					}
					else
					{
						fdc.paging.F3 = 0;
						fdc.paging.time = 0;
						fdc.paging.ca_id = 0;
						fdc.paging.ca_organization = 0;
						fdc.paging.paging_user_group =
								(static_cast<int>(data[5]) << 24)
								+ (static_cast<int>(data[6]) << 16)
								+ ((static_cast<int>(data[7])) << 8)
								+ (static_cast<int>(data[8]));
					}

				}
			}
			break;
			case TRAFFIC_MESSAGE_CONTROL:
			{
				if(fdc.D1 && (!fdc.D2)){
					size_t number_of_byte = 2;
					while(number_of_byte < size){
						fdc.traffic_Message.TMCmessage.push_back(
								(static_cast<uint16_t>(data[number_of_byte])<< 8)
								+ data[number_of_byte+1] );
						number_of_byte +=2;
					}
				}
			}
			break;
			case EMERGENCY_SYSTEM:
			{
				if (fdc.D2)
				{
					///@todo function to decode emergency messages
				}
				else
				{
					///@todo function to decode emergency messages
				}

			}
			break;
			default:
				break;
			}
			Info_FIG.FIC_data_channel.push_back(fdc);
		}
		break;
		case END_OR_RESERVE7:
		{
			FIG_extension = data[0] & 0x07;
			return FIG_extension;
		}
		break;
		default:
			break;
	}
	}
	return FIG_extension;

}

void DataDecoder::ExtractDataFromPacketMode(uint8_t* data, size_t size) {
}

void DataDecoder::CreateStation(std::list<stationInfo> * &station_info_list){
	/*
	 * CreateStations method. Builds stationInfo structures list for scheduler from scratch.
	 *
	 * "stat" is a temporary auxiliary stationInfo structure that will be filled with suitable data from other structures
	 * If MCI/FIGinfo structures lists are empty it means they weren't decoded.
	 * Bit flags are responsible to determine if suitable number of previous structures is achieved to add/update given structure.
	 * All the above conditions are being checked before assuming that given structure should be added/updated.
	 *
	 * Complete SUBCHANNEL_BASIC_INFORMATION (10). This is a basic structure indispensable for demodulator and MSCDecoder work.
	 */
	if (it_sbi->is_long){ ///< DAB+ mode
		stat.SubChannelId = it_sbi->subchannel_id;
		stat.station_name = "Not Available" ;
		stat.sub_ch_start_addr = static_cast<size_t>(it_sbi->start_address);
		stat.sub_ch_size = static_cast<size_t>(it_sbi->subchannel_size);
		stat.protection_level = it_sbi->protection_level;
		stat.ProtectionLevelTypeB = it_sbi->protection_level_typeB;
		stat.IsLong = it_sbi->is_long;
		stat.audio_kbps = 0;
		stat.ServiceId = 65000;

	}
	else{ //< DAB mode
		stat.SubChannelId = it_sbi->subchannel_id;
		stat.station_name = "Not Available" ;
		stat.sub_ch_start_addr = static_cast<size_t>(it_sbi->start_address);
		stat.sub_ch_size = Table6SubChannelShortInfo[it_sbi->table6_subchannel_short_info][1];
		stat.protection_level = Table6SubChannelShortInfo[it_sbi->table6_subchannel_short_info][2];
		stat.audio_kbps = Table6SubChannelShortInfo[it_sbi->table6_subchannel_short_info][3];
		stat.IsLong = it_sbi->is_long;
		stat.ServiceId = 65000;
		stat.ProtectionLevelTypeB = false;
	}

	/*
	 * Complete STATUS_BASIC_SERVICE_AND_SERVICE_COMPONENT (100)
	 */
	if ((fic_data_exist_status.MCI_status & 4) == 4){ ///< Check if BSASC(100) exist
		/*
		 * Compare to SUBCHANNEL_BASIC_INFORMATION by the suitable parameter
		 */
		it_bsasc = MCIdata.basic_Service_And_Service_Component.begin(); ///< set the itator pointer to the beginning of the list
		while (it_bsasc != MCIdata.basic_Service_And_Service_Component.end()){ ///< iterate until the end of the list
			if (it_sbi->subchannel_id == it_bsasc->subchannel_id[0]){ ///< compare SubChannelIds
				stat.ServiceId = it_bsasc->service_id; ///<get the data
			}
			++it_bsasc;
		}
	}
	/*
	 * Compare to SUBCHANNEL_BASIC_INFORMATION by the suitable parameter
	 */
	if (((fic_data_exist_status.MCI_status & 4) == 4) && ((fic_data_exist_status.labels1_status & 2) == 2)){ ///< Check if BSASC(100) and L1 and ensamble label exist
		/*
		 * Search LABELS1
		 */
		it_l = Info_FIG.labels1.begin(); ///< set iterator pointer to the beginning of the list
		while (it_l != Info_FIG.labels1.end()){ ///< iterate until the end of the list
			if (stat.ServiceId == it_l->FM_Text_Label.id_field){ ///< compare SubChannelIds
				stat.station_name = it_l->FM_Text_Label.label; ///<get the station name
			}
			++it_l;
		}
	}

	if (((fic_data_exist_status.MCI_status & 8) == 8)){ ///< Check if SCIPM(1000) exists
		/*
		 * Search service component in packet mode
		 */
		it_scipm = MCIdata.service_component_in_Packet_Mode.begin(); ///< set iterator pointer to the beginning of the list
		while (it_scipm != MCIdata.service_component_in_Packet_Mode.end()){ ///< iterate until the end of the list
			if (stat.SubChannelId == it_scipm ->sub_channel_id){ ///< compare SubChannelIds
				stat.station_name = "Packet Mode"; ///< station name == "Packet Mode"
			}
			++it_scipm;
		}
	}

	station_info_list->push_back(stat); ///< push to the station_info_list
}


void DataDecoder::UpdateStation(std::list<stationInfo>::iterator &it_sil){
	/*
	 * UpdateStations method. Updates stationInfo structures list for scheduler
	 *
	 * If MCI/FIGinfo structures lists are empty it means they weren't decoded.
	 * Bit flags are responsible to determine if suitable number of previous structures is achieved to add/update given structure.
	 * All the above conditions are being checked before assuming that given structure should be added/updated.
	 */
	/*
	 * Complete SUBCHANNEL_BASIC_INFORMATION (10). This is a basic structure indispensable for demodulator and MSCDecoder work.
	 */
	if ((fic_data_exist_status.MCI_status & 2) == 2){
		it_sbi = MCIdata.subChannel_Basic_Information.begin(); ///< set the list pointer to the beginning
		while(it_sbi != MCIdata.subChannel_Basic_Information.end()){
			if (it_sil->SubChannelId == it_sbi->subchannel_id){
				if (it_sil->IsLong){ ///< DAB+ mode
					it_sil->sub_ch_start_addr = static_cast<size_t>(it_sbi->start_address);
					it_sil->sub_ch_size = static_cast<size_t>(it_sbi->subchannel_size);
					it_sil->protection_level = it_sbi->protection_level;
					it_sil->ProtectionLevelTypeB = it_sbi->protection_level_typeB;
					it_sil->IsLong = it_sbi->is_long;
					it_sil->audio_kbps = 0;
				}
				else{ //< DAB mode
					it_sil->sub_ch_start_addr = static_cast<size_t>(it_sbi->start_address);
					it_sil->sub_ch_size = Table6SubChannelShortInfo[it_sbi->table6_subchannel_short_info][1];
					it_sil->protection_level = Table6SubChannelShortInfo[it_sbi->table6_subchannel_short_info][2];
					it_sil->audio_kbps = Table6SubChannelShortInfo[it_sbi->table6_subchannel_short_info][3];
					it_sil->IsLong = it_sbi->is_long;
				}
				MCIdata.subChannel_Basic_Information.erase(it_sbi++); ///< erase a station from the subChannel_Basic_Infrmation list
			}
			else{
				++it_sbi;
			}
		}
	}

	/*
	 * Complete STATUS_BASIC_SERVICE_AND_SERVICE_COMPONENT (100)
	 */
	if ((fic_data_exist_status.MCI_status & 4) == 4){ ///< Check if BSASC(100) exist
		/*
		 * Compare to SUBCHANNEL_BASIC_INFORMATION by the suitable parameter
		 */
		it_bsasc = MCIdata.basic_Service_And_Service_Component.begin(); ///< set the itator pointer to the beginning of the list
		while (it_bsasc != MCIdata.basic_Service_And_Service_Component.end()){ ///< iterate until the end of the list
			if (it_sil->SubChannelId == it_bsasc->subchannel_id[0]){ ///< compare SubChannelIds
				it_sil->ServiceId = it_bsasc->service_id; ///<get the data
			}
			++it_bsasc;
		}
	}

	/*
	 * Compare to SUBCHANNEL_BASIC_INFORMATION by the suitable parameter
	 */
	if ((fic_data_exist_status.labels1_status & 2) == 2){ ///< Check if  BSASC(100) and L1 and ensamble label exist
		/*
		 * Search STATUS_LABELS1 (100)
		 */
		it_l = Info_FIG.labels1.begin(); ///< set itator pointer to the beginning of the list
		while (it_l != Info_FIG.labels1.end()){ ///< iterate until the end of the list
			if (it_sil->ServiceId == it_l->FM_Text_Label.id_field){ ///< compare SubChannelIds
				it_sil->station_name = it_l->FM_Text_Label.label; ///<get the station name
			}
			++it_l;
		}
	}

	if (((fic_data_exist_status.MCI_status & 8) == 8)){ ///< Check if SCIPM(1000) exists
		/*
		 * Search service component in packet mode
		 */
		it_scipm = MCIdata.service_component_in_Packet_Mode.begin(); ///< set iterator pointer to the beginning of the list
		while (it_scipm != MCIdata.service_component_in_Packet_Mode.end()){ ///< iterate until the end of the list
			if (it_sil->SubChannelId == it_scipm ->sub_channel_id){ ///< compare SubChannelIds
				it_sil->station_name = "Packet Mode"; ///< station name == "Packet Mode"
			}
			++it_scipm;
		}
	}
}


void DataDecoder::StationsStatus(uint8_t FIG_Type, uint8_t  extract_FIC_return ){
	switch (FIG_Type){

	case MCI:{
		fic_data_exist_status.FIGtype_status |= 1;
		if((fic_data_exist_status.extract_FIC_return >=0) && (fic_data_exist_status.extract_FIC_return <= 31)){
			fic_data_exist_status.MCI_status |= (1 << fic_data_exist_status.extract_FIC_return);
		}
	}
	break;

	case LABELS1:{
		fic_data_exist_status.FIGtype_status |= (1 << 1);
		if((fic_data_exist_status.extract_FIC_return >=0) && (fic_data_exist_status.extract_FIC_return <= 1)){
			fic_data_exist_status.labels1_status |= (1 << fic_data_exist_status.extract_FIC_return);
		}
	}
	break;

	default:
		break;
	}

}


void DataDecoder::GetFicExtraInfo(UserFICData_t * user_fic_extra_data, stationInfo *audioService){
	/*
	 * GetFicExtraInfos method. Updates UserFICData_t structure with info about time, coordinates, xpads etc.
	 *
	 * If MCI/FIGinfo structures lists are empty it means they weren't decoded.
	 * Bit flags are responsible to determine if suitable number of previous structures is achieved to add/update given structure.
	 * All the above conditions are being checked before assuming that given structure should be added/updated.
	 *
	 */

	/*
	 * Copy FIC data status structure
	 */
	//user_fic_extra_data->user_fic_data_status = fic_data_exist_status;

	user_fic_extra_data->validity_ = 0;
	user_fic_extra_data->bitrate_ = audioService->audio_kbps;
	user_fic_extra_data->DAB_plus_ = audioService->IsLong;

	/*
	 * Get info about a InternationalTableId
	 */
	if((fic_data_exist_status.MCI_status & 512) == 512) { //FIG t0e9
		user_fic_extra_data->programme_type_ = MCIdata.country_LTO_and_International_table.international_table_id;
		user_fic_extra_data->validity_ |= UserFICData_t::PROGRAMME_TYPE_VALID;
	}

	/*
	 * Get info about data and time
	 */
	if((fic_data_exist_status.MCI_status & 1024) == 1024) { //FIG t0e10
		user_fic_extra_data->time_.h_ = MCIdata.date_and_time.hours;
		user_fic_extra_data->time_.ms_ = MCIdata.date_and_time.miliseconds;
		user_fic_extra_data->time_.m_ = MCIdata.date_and_time.minutes;
		user_fic_extra_data->time_.s_ = MCIdata.date_and_time.seconds;
		user_fic_extra_data->validity_ |= UserFICData_t::TIME_VALID;
	}

	/*
	 * Get info about coordinates
	 */
	if ((fic_data_exist_status.MCI_status & 2048) == 2048) { //FIG t0e11
		it_rd = MCIdata.region_definition.begin();// set the iterator pointer to the beginning of the list
		//both coordinates are stored as 2's complement --> they are signed
		user_fic_extra_data->latitude_ =
				static_cast<int16_t>(it_rd->coordinates.latitude_coarse);
		user_fic_extra_data->longitude_ =
				static_cast<int16_t>(it_rd->coordinates.longitude_coarse);
		user_fic_extra_data->validity_ |= UserFICData_t::COORDINATES_VALID;
	}

	/*
	 * check if LABELS1 (10) exists
	 */
	if ((fic_data_exist_status.FIGtype_status & 2) == 2) { //FIG t1

		/*
		 * check if LABELS1 fmtextlabel (10) exists
		 */
		it_l = Info_FIG.labels1.begin(); //set the iterator pointer to the beginning of the list
		if ((fic_data_exist_status.labels1_status & 2) == 2){ //FIG t0e1
			/*
			 * Search LABELS1 fmtextlabel (10)
			 */
			while (it_l != Info_FIG.labels1.end()){ // iterate until the end of the list
				if (audioService->ServiceId == it_l->FM_Text_Label.id_field){ // compare SubChannelIds
					/*
					 * complete charset info
					 */
					user_fic_extra_data->set_=it_l->charset;

					/*
					 * complete labels1 fmtextlabel info
					 */
					user_fic_extra_data->service_id_ = it_l->FM_Text_Label.id_field; //get the station id
					user_fic_extra_data->service_label_ = it_l->FM_Text_Label.label; //get the station name
					user_fic_extra_data->validity_ |= UserFICData_t::LABEL_VALID;
					break;
				}
				++it_l;
			}
			user_fic_extra_data->service_id_ = audioService->ServiceId;
		}
	}
}


void DataDecoder::DeViterbi(float* input, size_t size, uint8_t* output) {
	size_t NMemDepth;               		// 5*NDelays
	size_t Pos, mRead, mWrite, pLast;
	float Value1, Value2;
	size_t Ny4 = size/4;              		// 4-time redundancy
	size_t Ny4_8 = Ny4/8;					// output counter
	size_t index;
	size_t NStreams_3 = NStreams_-3;
	size_t bitShift = 7;

	NMemDepth = 5*NDelays_;          		// 30
	mRead = 0;
	mWrite = NStates_;
	pLast = NMemDepth-1;    				// the last (actual) position in PathBuff
	size_t NMemDepth_1 = NMemDepth-1;

	memset(accMetricBuff_, 0, sizeof(float)*128);	// ToDo: possible optimization  (not known if all need to be set to 0)
	zeroTail_[0] = input[size-4];			// copy tail of data to beginning of axu buffer.
	zeroTail_[1] = input[size-3];
	zeroTail_[2] = input[size-2];
	zeroTail_[3] = input[size-1];

	size_t outputCounter = 0;
	for(size_t i=0; i<Ny4+NMemDepth; ++i){    // for each input 4-element symbol (one row with 4 redundant values)
		int *inStat1_index = inStat1_;
		int *inStat2_index = inStat2_;
		int *refOut1tmp = refOut1_;
		int *refOut2tmp = refOut2_;
		float *yIntmp = input+i*NStreams_;
		for(index=0; index<NStates_; ++index ){

			if( i>=Ny4)
				yIntmp = zeroTail_;
			//else	//ToDo: possible optimization, Value1==Value2==0 because yIntmp==0

			// for each inner state 0:63
			if( *refOut1tmp++>0 )	Value1 = *yIntmp;				// fancy vector multiplication (speedUP)
			else					Value1 = -*yIntmp;
			if( *refOut2tmp++>0 )	Value2 = *yIntmp++;
			else					Value2 = -*yIntmp++;

			if( *refOut1tmp++>0 )	Value1 += *yIntmp;
			else					Value1 -= *yIntmp;
			if( *refOut2tmp++>0 )	Value2 += *yIntmp++;
			else					Value2 -= *yIntmp++;

			if( *refOut1tmp++>0 )	Value1 += *yIntmp;
			else					Value1 -= *yIntmp;
			if( *refOut2tmp++>0 )	Value2 += *yIntmp++;
			else					Value2 -= *yIntmp++;

			if( *refOut1tmp>0 )		Value1 += *yIntmp + accMetricBuff_[ *inStat1_index+mRead ];
			else					Value1 -= *yIntmp - accMetricBuff_[ *inStat1_index+mRead ];
			if( *refOut2tmp>0 )		Value2 += *yIntmp + accMetricBuff_[ *inStat2_index+mRead ];
			else					Value2 -= *yIntmp - accMetricBuff_[ *inStat2_index+mRead ];

			refOut1tmp +=  NStreams_3;
			refOut2tmp +=  NStreams_3;
			yIntmp -= 3;

			// Decision: what transition path is better?
			if( Value1 > Value2 ){  // EVEN path
				accMetricBuff_[index+mWrite] = Value1;
				pathBuff_[index][pLast] = *inStat1_index;
			}
			else                   // EVEN path
			{
				accMetricBuff_[index+mWrite] = Value2;
				pathBuff_[index][pLast] = *inStat2_index;
			}
			inStat1_index++;
			inStat2_index++;
		}

		// DECISION
		if( i > NMemDepth_1 ) {
			float *accMetricBufftmp = accMetricBuff_ + index+mWrite;
			Pos = 0;
			float posmax = *accMetricBufftmp;
			for(size_t x=1; x<NStates_; ++x ){
				if(*accMetricBufftmp > posmax){
					posmax = *accMetricBufftmp;
					Pos = x;
				}
				accMetricBufftmp++;
			}

			int pLast_NMemDepth = pLast+NMemDepth;
			for(size_t p=0; p<NMemDepth; ++p){                        // trace path back
				if(pLast<p) {
					Pos = pathBuff_[Pos][pLast_NMemDepth-p];           // read Pos for time "k-1"
				} else {
					Pos = pathBuff_[Pos][pLast-p];                     // read Pos for time "k-1"
				}
			}

			if(7==bitShift)						// clear output bytes
				*output = 0;

			if (Pos > NStates2_1_) {			// add ,,1''
				*output |= 1<<bitShift;
			}

			if(!bitShift){						// increment pointer to output buffer (next byte)
				outputCounter++;
				if( outputCounter >= Ny4_8 ){	// reach last byte
					return;
				}
				output++;
				bitShift=7;
			} else {
				bitShift--;
			}
		}

		// BUFFER CONTROL
		pLast++;
		if( pLast > NMemDepth_1)
			pLast=0;

		if( mRead==0 ) {
			mRead=NStates_;
			mWrite=0;
		} else {
			mRead=0;
			mWrite=NStates_;
		}
	}
}


void DataDecoder::DeViterbiInit(void) {
	NStreams_=4;
	NTaps_=7;
	NDelays_=6;
	NStates_=64;
	NStates2_=NStates_/2;
	NStates2_1_ = NStates_/2-1;

	int ii,iip;
	int addr;

	// Initialization of InState1 and InState2 matrices
	for(size_t i=0; i<NStates2_; i++)        // 0,1,2,...,31
	{
		ii = 2*i;
		inStat1_[i]=ii;
		inStat1_[i+NStates2_]=ii;       		// InStates1 = [ 0,2,4,...,62,  0,2,4,...,62]
		iip=ii+1;
		inStat2_[i]=iip;
		inStat2_[i+NStates2_]=iip;     		// InStates2 = [ 1,3,5,...,63,  1,3,5,...,63]
	}

	// Initialization of RefOut1 and RefOut2 matrices
	int NewBit;
	for(size_t i=0; i<NStates_ ;i++)         // for 0,1,2,3,...,63
	{
		ii=i*NStreams_;
		if(i<NStates2_){
			NewBit=0; 	// for 0,1,2,..., 31
		}
		else{
			NewBit=1;	// for 32,33,34,...,63
		}

		for(size_t k=0; k<NStreams_; k++)     // for 0,1,2,3
		{
			addr = k*NTaps_;   // address of the first element of the next rows of Generator

			const int *in1 = nextBits_ + NDelays_*inStat1_[i];
			const int *in2 = gen_ + addr;
			int acc = NewBit*in2[0];
			for(size_t x=1; x<NTaps_; ++x){
				acc += in1[x-1]*in2[x];
			}
			refOut1_[ii+k] = 1 - 2*( acc % 2); // multiply two NTaps=7-element vectors

			in1 = nextBits_ + NDelays_*inStat2_[i];
			in2 = gen_ + addr;
			acc = NewBit*in2[0];
			for(size_t x=1; x<NTaps_; ++x){
				acc += in1[x-1]*in2[x];
			}
			refOut2_[ii+k] = 1 - 2*( acc % 2); 	// multiply two NTaps=7-element vectors
		}
	}

	memset( zeroTail_, 0, sizeof(float)*120 );					// zero tail initialization
}

char *DataDecoder::EBULatinToUTF8(char ebu, char buf[4]) {
	int i = 0;
	unsigned char ebu_ = static_cast<unsigned char>(ebu);
	switch (ebu_) {
	//...
	///@todo more national letters from EBU table
	//...
	case 0x86: //LATIN SMALL LETTER O WITH ACUTE
	{
		buf[i++] = 0xC3;
		buf[i++] = 0xB3;
	}
	break;
	case 0xB6: //LATIN SMALL LETTER N WITH ACUTE
	{
		buf[i++] = 0xC5;
		buf[i++] = 0x84;
	}
	break;
	case 0xC6: //LATIN CAPITAL LETTER O WITH ACUTE
	{
		buf[i++] = 0xC3;
		buf[i++] = 0x93;
	}
	break;
	case 0xEB: //LATIN CAPITAL LETTER C WITH ACUTE
	{
		buf[i++] = 0xC4;
		buf[i++] = 0x86;
	}
	break;
	case 0xFB: //LATIN SMALL LETTER C WITH ACUTE
	{
		buf[i++] = 0xC4;
		buf[i++] = 0x87;
	}
	break;
	case 0xEC: //LATIN CAPITAL LETTER S WITH ACUTE
	{
		buf[i++] = 0xC5;
		buf[i++] = 0x9A;
	}
	break;
	case 0xFC: //LATIN SMALL LETTER S WITH ACUTE
	{
		buf[i++] = 0xC5;
		buf[i++] = 0x9B;
	}
	break;
	case 0xED: //LATIN CAPITAL LETTER Z WITH ACUTE
	{
		buf[i++] = 0xC5;
		buf[i++] = 0xB9;
	}
	break;
	case 0xFD: //LATIN SMALL LETTER Z WITH ACUTE
	{
		buf[i++] = 0xC5;
		buf[i++] = 0xBA;
	}
	break;

	default:
		//those have 1-1 relationship with UTF-8 (0x20-0x5D, 0x5F, 0x61-0x7D)
		buf[i++] = ebu;
		break;
	}

	buf[i] = '\0';
	return buf;
	/* (1) http://www.rthk.org.hk/about/digitalbroadcasting/DSBS/DABETS300401.PDF p.36
	 * (2) http://www.interactive-radio-system.com/docs/EN50067_RDS_Standard.pdf p.74
	 * (3) http://www.etsi.org/deliver/etsi_ts/101700_101799/101756/01.06.01_60/ts_101756v010601p.pdf p.42
	 * 1 references 2, but 3 is much clearer */
}
