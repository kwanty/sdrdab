/**
 * @class DataDecoder
 * @brief Data decoding - from dqpsk to ADTS AAC cointainer
 *
 * Puncturer, Viterbi, CRC, ReedSoloon, etc... . Need to work with MPEG and AAC codec.
 * Decode FIC - provide data for audio channel extraction (from MSC) as well as side data from FIC
 * (eg. Electronic Program Guide)
 *
 * @author Jaroslaw Bulat kwant@agh.edu.pl (DataDecoder, DataDecoder::CRC16, DataDecoder::BinToDec, DataDecoder::SuperFrameHandle, DataDecoder::SuperframeCircshiftBuff, DataDecoder::FirecodeInit, DataDecoder::FirecodeCheck)
 * @author Dawid Rymarczyk rymarczykdawid@gmail.com (DataDecoder::ExtractDataFromFIC, DataDecoder::EBULatinToUTF8)
 * @author Jan Twardowski (DataDecoder::ExtractDataFromPacketMode)
 * @author Adrian Karbowiak karbowia@student.agh.edu.pl (DataDecoder::DataDecoder, (DataDecoder::Process, DataDecoder::CreateStation, DataDecoder::UpdateStation, DataDecoder::StationsStatus, DataDecoder::GetFicExtraInfo), (DataDecoder::EnergyDispersal, DataDecoder::EnergyDispersalGen))
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


#ifndef DATADECODER_H_
#define DATADECODER_H_

/// @cond
#include <cstddef>
#include <stdint.h>
/// @endcond
#include "data_format.h"

class DataDecoder {
public:
	/**
	 * One argument constructor is used by scheduler in CONF state
	 * @param mode_parameters pointer to modeParameters structure from DataFormat.
	 */
	DataDecoder(modeParameters *mode_parameters);

	/**
	 * Two argument constructor is used by scheduler in PLAY state
	 * @param mode_parameters pointer to modeParameters structure from DataFormat.
	 * @param station_info pointer to stationInfo structure from DataFormat.
	 */
	DataDecoder(stationInfo *station_info, modeParameters *mode_parameters);


	virtual ~DataDecoder();

	/**
	 * Manage ,,logic'': data decoding, manage buffers, etc.
	 * @param decod pointer to scheduler buffer that contains data from demodulator (DQPSK)
	 * @param station_info_list pointer to list with radio stations parameters, names and extra data. Filled after FICdecoder.
	 * @param audioService current station chosen by user.
	 * @param[out] user_fic_extra_data Extra FIC data structure pointer
	 */
	void Process(decodReadWrite* decod, std::list<stationInfo> * station_info_list, stationInfo *audioService, UserFICData_t *&user_fic_extra_data);

	/// Tells what type of programme is now; 32 x 8 size
	static const char* InternationalProgrammeTable[][8];

	/// Tells what type of programme is now in Polish; 32 x 8 size
	static const char* InternationalProgrammeTablePolish[][8];

	/**
	 * Interprets FIG type 1 character set field.
	 * @param[in] charset charset to interpret
	 * @return FIG_type1_charset_t value corresponding to given charset
	 * @todo move to general API (somewhere?)
	 */
	inline static FIG_type1_set_t InterpretCharset(int charset) {
		switch (charset) {
		case FIG_SET_EBU_LATIN:
			return FIG_SET_EBU_LATIN;

		case FIG_SET_EBU_CYRILLIC:
			return FIG_SET_EBU_CYRILLIC;

		case FIG_SET_EBU_ARABIC:
			return FIG_SET_EBU_ARABIC;

		case FIG_SET_ISO_8859_2:
			return FIG_SET_ISO_8859_2;

		default:
			return FIG_SET_UNKNOWN;
		}
	}

	/**
	 * Translates given EBU Latin based repertoire (see ETSI TS 101 756 Annex C)
	 * byte to UTF-8 C-string.
	 * @param[in] ebu EBU Latin character
	 * @param[out] buf UTF-8 equivalent (1-3 useful character bytes + null byte).
	 * Must be able to store 4 chars.
	 * @return pointer to the beginning of buffer \c buf
	 */
	static char *EBULatinToUTF8(char ebu, char buf[4]);

#ifndef GOOGLE_UNIT_TEST
private:
#endif

	/// @name modeParameter & stationInfo structures
	///@{
	struct modeParameters mode_parameters;
	struct stationInfo station_info;
	///@}

	/// @name Depuncturer data
	///@{
	size_t devPI[24]; ///< matrix with devPI vectors (depuncturer)
	///@}
	const static size_t tmpDevPI[24]; ///< matrix with devPI vectors (depuncturer)

	/// @name Structure that contains key parameters for FICdecoder/MSCdecoder, especially depuncturer.
	///@{
	struct Depunctur_info {
		size_t lrange_fic[5];
		size_t lpi_fic[5];
		size_t audiolen_fic;
		size_t padding_fic;
		size_t after_depuncturer_total_len_fic;

		size_t lrange_msc[5];
		size_t lpi_msc[5];
		size_t audiolen_msc;
		size_t padding_msc;
		size_t after_depuncturer_total_len_msc;
	};
	struct Depunctur_info depunctur_info; ///< contains key parameters for FICdecoder/MSCdecoder, especially depuncturer.
	///@}

	/// @name Viterbi
	///@{
	int inStat1_[64];
	int inStat2_[64];
	int refOut1_[256];				// 64x4
	int refOut2_[256];				// 64x4
	int pathBuff_[64][30];        	// NStates=64, NMemDepth=30
	float accMetricBuff_[128];      	// 64*2, Nstates-64, always 2 (Old, New)
	float zeroTail_[120];			// 30x4, zero-tail
	///@}
	const static int gen_[28];
	const static int nextBits_[384];	//64x6
	/// @name Viterbi
	///@{
	size_t NStreams_;
	size_t NTaps_;
	size_t NDelays_;
	size_t NStates_;
	size_t NStates2_;
	size_t NStates2_1_;


	/// @name EnergyDispGen XOR vector pointer
	///@{
	uint8_t* energy_gen_data_fic; ///< energyDispGen pointer to store prbs data vector
	uint8_t* energy_gen_data_msc; ///< energyDispGen pointer to store prbs data vector

	///@}

	const static size_t pos_table[16]; ///<position table from ETSI, information how to copy input Data
	/// @name Variables for TimeDeinterleaver function
	///@{
	size_t idx_tab_size; ///<size for both tables of indices
	size_t CU_size; ///<Capacity Unit size 1CU = 64 bits
	size_t* in_idx_tab; ///<table of input indices
	size_t* out_idx_tab; ///<table of output indices
	size_t audio_size; ///<number of bits in 1 audio package. audio_size = sub_ch_size * CU_size
	size_t audio_and_pad_size; ///<number of bits in 1 audio package with padding. audio_and_pad_size = audio_size + padding.size
	size_t extra_pad_size; ///<additional auxiliary variable, padding.size multiplied by CIF counter in single TransmisionFrame
	size_t beg_pos; ///<initial begin position of CIF before copying in data buffer. beg_pos = 16
	size_t cur_pos; ///<current position of CIF after copying in data buffer. cur_pos depends on position table from ETSI
	size_t beg_fic_size; ///<size of all FICs between 0 and beg_pos in data buffer. beg_fic_size = beg_pos / number_of_cif * fic_size;
	size_t cur_fic_size; ///<size of all FICs between 0 and cur_pos + CIF_count in data buffer. cur_fic_size = (cur_pos + FIC_count) / number_of_cif * fic_size;

	///@}

	/// @name MSC iterators
	///@{
	struct Msc_decoder_info{
		size_t cif_count;
		size_t number_dab_frame;
		bool super_frame_sync;
		uint8_t adts_dacsbr;
		uint8_t adts_chanconf;
		size_t number_bits_per_cif;
		size_t number_bits_per_frame;

	};
	struct Msc_decoder_info msc_info;
	///@}

	/// @name FICDecoder/MSCDecoder data
	///@{
	float *input_fic_decoder; ///< FICdecoder - input
	float *depunctur_data_;		///< depunctured data (ring buffer is not necessary!)
	uint8_t *binary_data_;		///< ring buffer of binary output buffer
	float *depunctur_data_msc;
	uint8_t *binary_data_msc; ///< ring buffer of binary output buffer
	uint8_t* energ_disp_out;
	size_t super_frame_size_;

	///@}

	/// @name Structure for Process that contains info about demodulator buffer paddings
	///@{
	struct Padding{
		size_t leftPaddingOffset; ///< the first padding from left to right that occurs in dQPSK buffer, right after FIC
		size_t rightPaddingOffset; ///< the second padding from left to right that occurs in the same dQPSK buffer, right after audio
		size_t size;
	};
	struct Padding padding;
	///@}

	/// @name ExtractFIC data
	/// @brief Includes all informations about multiplex and services
	///@{
	struct MCI MCIdata; ///< subchannel basic parameters, linking parameters with infoFIG
	struct InfoFIG Info_FIG; ///< station names etc.
	///@}

	/**
	 * Table 6 from DAB standard {Index,SubChannelSize,ProtectionLevel,BitRate(kbit\\s)}.
	 * Tells what are parameters of station in DAB not in DAB+.
	 * 64 x 4 size
	 */
	static const int Table6SubChannelShortInfo[][4];

	/// @name MCI, FIGinfo lists iterator pointers declarations for CreateStation/UpdateStation
	///@{
	struct Fic_Data_Exist_Status fic_data_exist_status; ///< structure keeps info about status of FIC structures
	struct stationInfo stat; ///< temporary stationInfo structure
	std::list<MCI::SubChannel_Basic_Information>::iterator it_sbi; ///< SubChannel_Basic_Infrmation list pointer
	std::list<MCI::Basic_Service_And_Service_Component>::iterator it_bsasc; ///< Basic_Service_And_Service_Component list pointer
	std::list<InfoFIG::Labels1>::iterator it_l; ///< InfoFIG::Labels1 list pointer
	std::list<stationInfo>::iterator it_sil; ///< stationInfo list pointer
	std::list<MCI::Region_definition>::iterator it_rd; ///< region list pointer
	std::list<MCI::Service_component_in_Packet_Mode>::iterator it_scipm;
	///@}

	/**
	 * high level decoding FIC - Fast Information Channel
	 * @param data pointer to samples_
	 * @todo common parts with MSCDecoder()
	 */
	void FICDecoder(float *data);

	/**
	 * high level decoding MSC - Main Service Channel
	 * extract data from MSC to MPEG and AAC
	 * @param data pointer to samples_
	 * @param read_size size of *data
	 * @param write_data output buffer
	 * @param is_dab DAB/DAB+ mode, true if DAB
	 * @todo common parts with FICDecoder()
	 * @todo not sure if transmission Mode is enough
	 * (eg. different bitrate, many audio streams)
	 * @todo prepare workflow for decoding many audio streams simultanously
	 */
	void MSCDecoder(float *data, size_t read_size, uint8_t* write_data, bool is_dab);

	/**
	 * Formating MSC frame (time interleaving), in place processing.
	 * Copies audio bits to: provide redundancy, allow data repair from defective Transmission Frame.\n
	 * It works: in stream mode, with single copying procedure, without extra buffers, using vectors of indices to copy bits in data buffer.\n
	 * It requires: access to multiple TF at the same time, to shift the buffer every approaching TF.
	 * @author Ernest Biela
 	 * @param data pointer to samples_
	 * @todo combine with DataDecoder::DePuncturer (all possible combination of DAB modes and bitrates)
	 */
	void TimeDeInterleaver(float *data);

	/**
	 * DePuncturerInitialization - FIC
	 */
	void DePuncturerFICInit();

	/**
	 * DePuncturerInitialization - MSC
	 */
	void DePuncturerMSCInit(size_t kbps_sbchsize, uint8_t protection, bool uep, bool isBProtection);

	/**
	 * Depuncturing, create output buffer. input buffer is complex float (eg. float interleaved),
	 * output is float (real,imag -> real,real) 4x bigger because of depuncturing. Different modes for: DAB, DAB+, FIC
	 * @param data pointer to samples_
	 * @param datalen length of data to read
	 * @param depunctur pointer to output data provided by Process()
	 * @param msc
	 * @param uep
	 * @todo is it possible avoid creating new buffer?
	 * @todo provide FIC/MSC mode, transmissionMode is not enough
	 * @todo is it possible to combine depuncturing with deinterleaving? (all possible combination of DAB modes and bitrates)
	 */
	void DePuncturer(const float *data, size_t datalen, float *depunctur, bool msc, bool uep);


	/**
	 * performe Viterbi algorithm
	 * @param input pointer to input data, one ,,float'' one bit
	 * @param size size of input data FIC/MSC
	 * @param output pointer to output data, package in bytes, first input bit == most significant bit in first output byte
	 */
	void DeViterbi(float *input, size_t size, uint8_t *output);

	/**
	 * initialize tables for DAB AAC+ Viterbi
	 */
	void DeViterbiInit( void );

	/**
	 * CRC verification, only for FIC
	 * @param data pointer to binary_data_
	 * @param size size of data
	 * @return true if CRC is ok, false if data are corrupted
	 * @todo should be litle/big endian save - not tested!
	 */
	bool CRCCheck(uint8_t *data, size_t size);

	/**
	 * Firecode CRC, implementation from GNU Radio, updated by sdr-j
	 * @param data pointer to binary_data_
	 * @return true if CRC is ok, false if data are corrupted
	 * @todo make own implementation, add FireCodeRepair
	 */
	bool FirecodeCheck(const uint8_t *data);

	/**
	 * initialization of FireCode
	 */
	void FirecodeInit(void);

	uint16_t firecode_tab[256];				/// table for firecode
	static const uint8_t firecode_g[16];	/// table for firecode

	/**
	 * Creates a vector that is used in EnergyDispersal to XOR with data
	 * @param energy_dispersal_table precomputed table for energy dispersal
	 * @param size of the vector (size in bytes)
	 */
	void EnergyDispersalGen(uint8_t* energy_dispersal_table, size_t size);

	/**
	 * Descrambler (XOR), in-place operation.
	 * @param data pointer to binary_data_ (bytes)
	 * @param energyGenData generated in EnergyDispGen method. It's being created once in a lifetime of the DataDecoder class object
	 * @param output in bytes
	 * @param disp_gen_size of the EnergyDispGen vector (size in bytes)
	 */
	void EnergyDispersal(uint8_t* data, uint8_t*energyGenData, uint8_t* output, size_t disp_gen_size);

	/**
	 * Super frame creation AAC
	 * @param data pointer to input binary_data_
	 * @param write_data pointer to output data, length is data dapendent (related to bitrate)
	 */
	void SuperFrameHandle(uint8_t *data, uint8_t* write_data);

	size_t superframe_capacity_;/// number of cifs could be stored in internal buffer (superframe)
	size_t superframe_cifs_;	/// number of cifs in superframe
	uint8_t	*superframe_;		/// superframe buffer
	size_t adts_head_idx_;		/// adts starts at given cif (valid if adts_synchro_)

	/**
	 * conversion from binary to dec, binary could start in any bit (in the middle of byte)
	 * and could span to any bytes (usually 12-14 bits)
	 * @param data pointer to input data (bytes)
	 * @param offset to first (most significant) bit, not necessary in first byte
	 * @param length number of bits to conversion (starts from offset)
	 * @return converted data
	 */
	uint16_t BinToDec( uint8_t *data, size_t offset, size_t length );

	/**
	 * CRC16 algorithm
	 * @param data pointer to input data (bytes)
	 * @param length number of bytes to conversion
	 */
	bool CRC16( uint8_t* data, size_t length );

	/**
	 * sirc-shift internalbuffer
	 * @param data - buffer for circshift
	 */
	void SuperframeCircshiftBuff(uint8_t *data);

	/**
	 * @author: Babiuch Michal
	 * Reed-Solomon correction:
	 * Correction of 120-Bytes packs with additional 10 parity Bytes. Here, decodes 1680B-long array (14*120B)
	 * into 1540B-long array (14*110B).
	 * Only for MSC and DAB+ (do not exist in DAB/MPEG?)
	 * create output buffer containing - ADTS container. In place operation. Use library,
	 * VERY OPTIONAL ;-) for own implementation.
	 * @param[in,out] data Pointer to array of data passed to method
	 * @param[in] size Size of 'data' array passed to method (here 1680)
	 * @return Returns number of errors corrected in each 1680B pack
	 * @todo create another buffer audio_bitstream_ or put data in binary_data_ (data do not change size)
	 * data from buffer are provide to AudioDecoder
	 */
	size_t ReedSolomonCorrection(uint8_t *data, int32_t size);

	/**
	 * Extraction parameters and settings from FIC. Decode position of audio stream as well as
	 * side data like Electronic Data Guide (tags)
	 * @param data pointer to binary_data_
	 * @param size number of bytes
	 * @param FIG_Type of FIG - is that MIC Labels or other data
	 * @return information about which data was extracted
	 */
	uint8_t ExtractDataFromFIC(uint8_t *data, size_t size, uint8_t FIG_Type);

	/**
	 * Optional. Decode data (eg. imagesc) from so called Packet Mode
	 * @param data pointer to binary_data_
	 * @param size number of bytes (?)
	 */
	void ExtractDataFromPacketMode(uint8_t *data, size_t size);

	/**
	 * CreateStations method. Builds stationInfo structures list for scheduler from scratch
	 * @param station_info_list pointer to stationInfo list
	 */
	void CreateStation(std::list<stationInfo> * &station_info_list);

	/**
	 * UpdateStations method. Updates stationInfo structures list for scheduler
	 * @param it_sil iterator pointer to stationInfo list
	 */
	void UpdateStation(std::list<stationInfo>::iterator &it_sil);

	/**
	 * Update stations' structures status (if empty or full of proper data)
	 * @param FIG_Type type of FIG structure (MCI/labels1...)
	 * @param extract_FIC_return - current return value from extract data from fic
	 */
	void StationsStatus(uint8_t FIG_Type, uint8_t  extract_FIC_return );

	/**
	 * Set extra FIC data: time, coordinates, xpads, etc.
	 * @note User application is responsible for calling delete [], when the
	 * structure is no longer needed.
	 * @param user_fic_extra_data pointer to UserFICData_t structure.
	 * @param audioService pointer
	 */
	void GetFicExtraInfo(UserFICData_t * user_fic_extra_data, stationInfo *audioService);

};

#endif /* DATADECODER_H_ */
