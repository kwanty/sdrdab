/*
 * @brief create, synchronize and keep running SDR-DAB
 *
 * Take care about whole workflow. Start process in single or multithread mode
 * set parameters, perform State Machine, communicate with user space (eg. send/print tags from FIC)
 *
 * @author Jaroslaw Bulat kwant@agh.edu.pl (Scheduler)
 * @author Sebastian Lesniak sebastian.lesniak@outlook.com (Scheduler, all methods)
 * @date 7 July 2015
 * @version 1.0 beta
 * @copyright Copyright (c) 2015 Jaroslaw Bulat, Sebastian Lesniak
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

#include "scheduler.h"
/// @cond
#include <cstring>
#include <cstdlib>
/// @endcond
#include <pthread.h>
#include <sys/time.h>
#include <fstream>


Scheduler::Scheduler() :

	datafeeder_(NULL),
	synchronizer_(NULL),
	demodulator_(NULL),
	datadecoder_(NULL),
	audiodecoder_(NULL),

	sync_feeder_buffer_(NULL),
	demod_decod_buffer_(NULL),
	audio_buffer_(NULL),

	demod_decod_buffer_multiplier_(3),
	sync_feeder_buffer_multiplier_(9), // Changing sync_feeder_buffer_ size muliplier (5 frames from DAB_MODE_I)
	audio_buffer_multiplier_(1),

	sync_feeder_buffer_size_((sync_feeder_buffer_multiplier_ * 196874)),
	demod_decod_buffer_size_(0),
	audio_buffer_size_(0),

	data_write_position_(0),
	sync_read_position_(0),
	demod_read_position_(0),
	demod_write_position_(0),
	decod_read_position_(0),
	decod_write_position_(0),
	audio_read_position_(0),

	deqpsk_unit_size_(0),
	deqpsk_block_size_(0),
	decod_write_size_(0),

	sync_read_size_(0),
	sync_pointer_shift_size_(0),
	sync_feeder_buffer_end_data_(0),

	fs_drift_(0),
	fs_counter_(0),
	sync_frame_size_(0),
	previous_null_(0),
	current_null_(0),
	lost_frames_(0),

	verbose_(0),

	state_(INIT),
	requested_stop_(false),
	sync_counter_(0),

	play(0),
	stored(0),
	number_of_frames_in_sync_(10),
	program_end_delay_(0),
	counter_(0),
	pointer_verbose_(0),
	sync_feeder_counter_(0),
	demod_counter_(0),
	decod_counter_(0),

	number_of_frames_for_calculate_fsdrift_(10),
	set_fs_drift_(true),
	demod_decod_created_(false),
	station_number_(255),

	datafeeder_lock_buffer_(),
	datafeeder_startprocessing_thread_(), // DataFeeder StartProcessing() thread
	mode_parameters_(), // Parameters of mode transmission
	sync_detect_(),
	sync_read_(),
	data_write_(),
	demod_read_write_(),
	decod_read_write_(),
	sync_feedback_(),
	audio_read_(),
	station_info_list_(),
	station_info_(),
	audiothread_(),
	use_pulse_sink_(),
	pulse_sink_(),
	output_filename_(NULL),
	ogg_sink_(),
	station_found_(true)

{
}

void Scheduler::SchedulerHardTest()
{

	// *** INIT *** //
	sync_feeder_buffer_size_ = 20 * 196000;
	sync_feeder_buffer_ = new float[sync_feeder_buffer_size_]();

    std::fstream myfile("/home/seba/gitdev/sdr/sdrdab/test/testdata/antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw", std::ios_base::in);

    for (unsigned i = 0; i < 20 * 196000; i++)
    	myfile >> sync_feeder_buffer_[i];



	sync_detect_.mode = DAB_MODE_UNKNOWN;
	station_info_.station_name = "nonexistent";
	sync_read_.read_here = sync_feeder_buffer_;
	sync_read_.read_size = 600000; // in SYMBOLS
	sync_read_size_ = 1200000; // in NUMBERS
	synchronizer_ = new Synchronizer(sync_read_.read_size);


	// *** SYNC *** //

	// TUTAJ POROWANIE DANYCH
	synchronizer_->DetectMode( sync_read_.read_here, sync_read_.read_size, &sync_detect_ );

	SetModeParameters( sync_detect_.mode );

	sync_read_.read_here = sync_read_.read_here + 2 * sync_detect_.null_position - mode_parameters_.null_size + 2 * mode_parameters_.frame_size;
	sync_read_position_ = sync_read_position_ + 2 * sync_detect_.null_position - mode_parameters_.null_size + 2 * mode_parameters_.frame_size;

	sync_read_.read_size = mode_parameters_.sync_read_size;
	sync_read_size_ = 2 * mode_parameters_.sync_read_size;
	sync_pointer_shift_size_ = 2 * mode_parameters_.frame_size;

	decod_write_size_ = mode_parameters_.number_of_cif * 384;

	delete synchronizer_;
	synchronizer_ = new Synchronizer(&mode_parameters_, mode_parameters_.sync_read_size);

	// TUTAJ POROWANIE DANYCH
	synchronizer_->Process( sync_read_.read_here, sync_read_.read_size, &sync_feedback_ );

	if ( ( static_cast<int>(sync_read_position_) + 2 * static_cast<int>(sync_feedback_.null_position) - static_cast<int>(mode_parameters_.null_size) ) > 0 )
	{
		sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
		sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
	}

	sync_read_.read_here += sync_pointer_shift_size_;
	sync_read_position_ += sync_pointer_shift_size_;

	// *** CONF *** //

	// TUTAJ POROWANIE DANYCH
	synchronizer_->Process( sync_read_.read_here, sync_read_.read_size, &sync_feedback_ );

	if ( ( static_cast<int>(sync_read_position_) + 2 * static_cast<int>(sync_feedback_.null_position) - static_cast<int>(mode_parameters_.null_size) ) > 0 )
	{
		sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
		sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
	}

	demodulator_ = new Demodulator(&mode_parameters_);
	datadecoder_ = new DataDecoder(&mode_parameters_);
	demod_decod_buffer_size_ = 2 * mode_parameters_.fic_size;
	demod_decod_buffer_ = new float[demod_decod_buffer_size_]();

	demod_read_write_.write_here = demod_decod_buffer_;

	decod_read_write_.read_here = demod_decod_buffer_;
	decod_read_write_.read_size = mode_parameters_.fic_size;

	demod_read_write_.read_here = sync_read_.read_here + 3 * mode_parameters_.null_size; // demodulator reads at end of null
	demod_read_position_ = sync_read_position_ + 3 * mode_parameters_.null_size;
	// TUTAJ POROWNANIE DANYCH
	demodulator_->Process( &station_info_, &demod_read_write_ );

	UserFICData_t *user_data = NULL;
	datadecoder_->Process( &decod_read_write_, &station_info_list_, &station_info_, user_data );
	delete user_data; //not needed at this step

	audiodecoder_ = new AudioDecoder(0.1, 50 * 10000);
	pulse_sink_ = new PulseSink();
	audiodecoder_->AddSink(pulse_sink_);
	///@todo not sure if those should be removed

	audio_buffer_size_ = audio_buffer_multiplier_ * 2000;
	audio_buffer_ = new uint8_t[audio_buffer_size_]();
	number_of_frames_for_calculate_fsdrift_ = 10;

	sync_read_.read_here += sync_pointer_shift_size_;
	sync_read_position_ += sync_pointer_shift_size_;


	// *** CONFSTATION *** //

	std::list<stationInfo>::iterator it;
	for ( it=station_info_list_.begin(); it != station_info_list_.end(); ++it )
	{
		if(it->SubChannelId == station_number_)
			break;
	}
	station_info_.SubChannelId = it->SubChannelId;
	station_info_.station_name = it->station_name;
	station_info_.sub_ch_start_addr = it->sub_ch_start_addr;
	station_info_.sub_ch_size = it->sub_ch_size;
	station_info_.protection_level = it->protection_level;
	station_info_.ProtectionLevelTypeB = it->ProtectionLevelTypeB;
	station_info_.IsLong= it->IsLong;
	station_info_.ServiceId = it->ServiceId;
	station_info_.audio_kbps = it->audio_kbps;

	delete datadecoder_;
	datadecoder_ = new DataDecoder(&station_info_, &mode_parameters_);

	CalculateDeqpskBuffer();

	demod_read_write_.write_here = demod_decod_buffer_ + 4 * deqpsk_unit_size_;
	demod_write_position_ = 4 * deqpsk_unit_size_;

	decod_read_write_.read_here = demod_decod_buffer_;
	decod_read_write_.read_size = deqpsk_block_size_;
	decod_read_write_.write_here = audio_buffer_;

	// *** PLAY *** //

	for(unsigned i = 0; i < 5; i++)
	{
		synchronizer_->Process( sync_read_.read_here, mode_parameters_.sync_read_size, &sync_feedback_ );

		if( ( static_cast<int>(sync_read_position_) + 2 * static_cast<int>(sync_feedback_.null_position) - static_cast<int>(mode_parameters_.null_size) ) > 0 )
		{
			sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
			sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
		}


		demod_read_write_.read_here = sync_read_.read_here + 3 * mode_parameters_.null_size; // demodulator reads at end of null
		demod_read_position_ = sync_read_position_ + 3 * mode_parameters_.null_size;

		demodulator_->Process( &station_info_, &demod_read_write_ );

		sync_read_.read_here += sync_pointer_shift_size_;
		sync_read_position_ += sync_pointer_shift_size_;

		UserFICData_t *user_data = NULL;
		datadecoder_->Process( &decod_read_write_, &station_info_list_, &station_info_, user_data );

		decod_read_write_.read_here += deqpsk_unit_size_;
		decod_read_position_ += deqpsk_unit_size_;
		demod_read_write_.write_here += deqpsk_unit_size_;
		demod_write_position_ += deqpsk_unit_size_;

		if(demod_write_position_ + deqpsk_unit_size_ > demod_decod_buffer_size_)
		{
			memcpy(demod_decod_buffer_, demod_decod_buffer_ + demod_decod_buffer_size_ - deqpsk_block_size_ + deqpsk_unit_size_,
					sizeof(float) * (deqpsk_block_size_ - deqpsk_unit_size_));
			demod_read_write_.write_here -= decod_read_position_ ;
			demod_write_position_ -= decod_read_position_ ;
			decod_read_write_.read_here -= decod_read_position_;
			decod_read_position_ = 0;
		}
	}


}

static void * AudioProcess(void *data) {

	audiodecoderData * audio_data = reinterpret_cast<audiodecoderData*>(data);
	AudioDecoder * audiodecoder = reinterpret_cast<AudioDecoder*>(audio_data->audio_decoder);

	// Play music
	while( !audio_data->finish_work )
		audiodecoder->Process();

	return NULL;
}


Scheduler::state_t Scheduler::Init(const char * dongle_or_file_name, uint8_t internal_buffer_number, size_t internal_buffer_size, uint32_t sample_rate,
		             uint32_t carrier_freq, data_source_t data_source) {

	// Creating DataFeeder object, for file or for dongle
	if ( data_source == DATA_FROM_FILE )
	{
		datafeeder_ = new FileDataFeeder( dongle_or_file_name, internal_buffer_size, sample_rate, carrier_freq );
	}
	else if ( data_source == DATA_FROM_DONGLE )
	{
		datafeeder_ = new DataFeeder( dongle_or_file_name, internal_buffer_number, internal_buffer_size, sample_rate, carrier_freq );
	}
	else
	{
		return INTERNAL_ERROR;
	}

	if ( verbose_ )
		datafeeder_->VerbosityOn();
	else
		datafeeder_->VerbosityOff();

	// If DataFeeder created properly, set parameters for him and create StartProcessing() thread for writing data
	if ( datafeeder_->EverythingOK() )
	{
		sync_feeder_buffer_ = new float[sync_feeder_buffer_size_]();
		data_write_.block_size = internal_buffer_size;
		data_write_.blocks_skipped = 0;
		data_write_.data_stored = false;
		data_write_.finish_rtl_process = false;
		data_write_.lock_buffer = &datafeeder_lock_buffer_;
		data_write_.object = datafeeder_;
		data_write_.write_here = sync_feeder_buffer_;

		if( pthread_mutex_init(&datafeeder_lock_buffer_,NULL) )
		{
			return INTERNAL_ERROR; //Error in INIT state: mutex_init error
		}

		if( !pthread_create(&datafeeder_startprocessing_thread_,NULL,StartProcessing,&data_write_) )
		{
			for(uint8_t i = 0; i < 100; i++)
			{
				if(datafeeder_->IsRunning())
					break;

				usleep(1);
				if(i >= 100)
				{
					return INTERNAL_ERROR; //EverythingOK() is set to one, but IsRunning() is still set to 0 (should not happen)
				}
			}

		}
		else
		{
			return INTERNAL_ERROR;
		}

		// Create Synchronizer object
		sync_detect_.mode = DAB_MODE_UNKNOWN;
		station_info_.station_name = "nonexistent";
		sync_read_.read_here = sync_feeder_buffer_;
		sync_read_.read_size = 600000; // in SYMBOLS
		sync_read_size_ = 1200000; // in NUMBERS
		synchronizer_ = new Synchronizer(sync_read_.read_size);

		// Go to SYNC state
		state_ = SYNC;
	}
	else
	{
		state_ = INIT_ERROR;
	}

	return state_;

}

Scheduler::state_t Scheduler::Sync( data_source_t data_source )
{
	bool is_locked = false;
	// If DataFeeder stored data, lock mutex, set pointer to end of wrote data.
	if(data_write_.data_stored)
	{
		pthread_mutex_lock( data_write_.lock_buffer );
		SetNewWritePointer();
		is_locked = true;
	}

	// When DataFeeder writes enough data to start processing frame
	if ( data_write_position_ > sync_read_position_ + sync_read_size_ )
	{
		// If mode not detected, try to detect it again
		if ( sync_detect_.mode == DAB_MODE_UNKNOWN )
		{

			if ( pointer_verbose_ )
			{
				FILE * pFile;
				pFile = fopen ( "tmp/sync_pointer_debug", "w" );
				fprintf( pFile, "Buffer start address: %p, Buffer end address: %p\n", sync_feeder_buffer_, sync_feeder_buffer_ + sync_feeder_buffer_size_ );
				fprintf( pFile, "Synchronizer::DetectMode(), counter: %zu, read address:  %p, read + size: %p\n", sync_feeder_counter_, sync_read_.read_here, sync_read_.read_here + sync_read_size_ );
				fclose ( pFile );
				sync_feeder_counter_++;
			}
			// Detect mode and return null position
			synchronizer_->DetectMode( sync_read_.read_here, sync_read_.read_size, &sync_detect_ );

			// If mode not detected, shift pointer to new data
			if ( sync_detect_.mode == DAB_MODE_UNKNOWN )
			{
				sync_read_.read_here += sync_read_size_;
				sync_read_position_ += sync_read_size_;

				ParametersFromSDR(DAB_NOT_DETECTED);
			}
			else
			{
				// If mode detected, set proper parameters for transmission mode, and proper pointers position

				if ( data_source == DATA_FROM_FILE )
				{
					if ( sync_detect_.mode == DAB_MODE_II || sync_detect_.mode == DAB_MODE_III)
						data_write_.block_size = 3 * 16384;
					else if ( sync_detect_.mode == DAB_MODE_IV )
						data_write_.block_size = 6 * 16384;

					number_of_frames_for_calculate_fsdrift_ = 3;
				}

				SetModeParameters( sync_detect_.mode );

				if ( verbose_ )
					fprintf( stderr, "Mode: %d, null: %zu \n", sync_detect_.mode, sync_detect_.null_position );

				sync_read_.read_here = sync_read_.read_here + 2 * sync_detect_.null_position - mode_parameters_.null_size + 2 * mode_parameters_.frame_size;
				sync_read_position_ = sync_read_position_ + 2 * sync_detect_.null_position - mode_parameters_.null_size + 2 * mode_parameters_.frame_size;

				sync_read_.read_size = mode_parameters_.sync_read_size; // setting read size for synchronizer = frame_size + null size
				sync_read_size_ = 2 * mode_parameters_.sync_read_size;
				sync_pointer_shift_size_ = 2 * mode_parameters_.frame_size;

				// Delete Synchronizer, create it for work in detected mode
				delete synchronizer_;
				synchronizer_ = new Synchronizer(&mode_parameters_, mode_parameters_.sync_read_size);
			}
		}
		// If mode detected, sync until we will be able to decode FIC
		else
		{
			if ( pointer_verbose_ )
			{
				FILE * pFile;
				pFile = fopen ( "tmp/sync_pointer_debug", "a" );
				fprintf( pFile, "Synchronizer::Process() (in SYNC), counter: %zu, read address:  %p, read + size: %p\n", sync_feeder_counter_, sync_read_.read_here, sync_read_.read_here + sync_read_size_ );
				fclose( pFile );
				sync_feeder_counter_++;
			}

			// Sync frame, detect null position and its quality, calcualte fc_drift
			synchronizer_->Process( sync_read_.read_here, sync_read_.read_size, &sync_feedback_ );

			if ( verbose_ )
				fprintf( stderr, "Fc drift: %f \n", sync_feedback_.fc_drift );

			if ( sync_feedback_.null_quality == NULL_OK )
			{
				if ( verbose_ )
					fprintf( stderr, "Null in sync_ok: %zu \n", sync_feedback_.null_position );

				// Correct pointer position
				if ( ( static_cast<int>(sync_read_position_) + 2 * static_cast<int>(sync_feedback_.null_position) - static_cast<int>(mode_parameters_.null_size) ) > 0 )
				{
					sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
					sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
				}

				CalculateAndSetDrifts();

				// If there is no need to correct fs_drift, go to CONF state and try decode FIC
				if ( !set_fs_drift_ )
				{
					state_ = CONF;
					if ( verbose_ )
						fprintf(stderr, "GOING TO CONF STATE\n");
				}

				sync_read_.read_here += sync_pointer_shift_size_;
				sync_read_position_ += sync_pointer_shift_size_;

			}
			else if ( sync_feedback_.null_quality == NULL_SHIFT )
			{
				if( verbose_ )
					fprintf( stderr, "Null in sync_shift: %zu \n", sync_feedback_.null_position );

				// Reset calculating fs_drift, shift pointer
				fs_drift_ = 0;
				fs_counter_ = 0;
				sync_frame_size_ = 0;

				sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size + sync_pointer_shift_size_;
				sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size + sync_pointer_shift_size_;

			}
			else if ( sync_feedback_.null_quality == NULL_NOT_DETECTED )
			{
				if ( verbose_ )
					fprintf( stderr, "******************* NULL not detected *************************\n" );

				// Reset calculating fs_drift
				ParametersFromSDR(DAB_NOT_DETECTED);

				fs_drift_ = 0;
				fs_counter_ = 0;
				sync_frame_size_ = 0;

				sync_read_.read_here += sync_pointer_shift_size_;
				sync_read_position_ += sync_pointer_shift_size_;

			}
			else
			{
				if ( verbose_ )
					fprintf(stderr, "Error in Sync method, undefined null value");
			}
		}
	}

	// If mutex is locked, calculate new pointer position for writing, unlock mutex
	if(is_locked)
	{
		CalculateNewBufferPosition( ( data_write_.blocks_skipped + 1 ) * data_write_.block_size );
		data_write_.data_stored = false;

		if ( pthread_mutex_unlock( data_write_.lock_buffer ) )
		{
			if ( verbose_ )
				fprintf( stderr, "Can't unlock mutex in SYNC method (deadlock may occur)\n" );
		}
	}

	return state_;
}


Scheduler::state_t Scheduler::Conf()
{
	bool is_locked = false;

	// If DataFeeder stored data, lock mutex, set pointer to end of wrote data.
	if(data_write_.data_stored)
	{
		pthread_mutex_lock( data_write_.lock_buffer );
		SetNewWritePointer();
		is_locked = true;
	}

	// When DataFeeder writes enough data to start processing frame
	if ( data_write_position_ > sync_read_position_ + sync_read_size_ )
	{
		if ( pointer_verbose_)
		{
			FILE * pFile;
			pFile = fopen( "tmp/sync_pointer_debug", "a" );
			fprintf( pFile, "Synchronizer::Process() (in CONF), counter: %zu, read address:  %p, read + size: %p\n", sync_feeder_counter_, sync_read_.read_here, sync_read_.read_here + sync_read_size_ );
			fclose( pFile );
			sync_feeder_counter_++;
		}

		// Sync frame, detect null position and its quality, calcualte fc_drift
		synchronizer_->Process( sync_read_.read_here, mode_parameters_.sync_read_size, &sync_feedback_ );

		if ( sync_feedback_.null_quality == NULL_OK )
		{
			if ( verbose_ )
				fprintf( stderr, "Null in conf_ok: %zu \n", sync_feedback_.null_position );

			// Correct pointer position
			if ( (static_cast<int>(sync_read_position_) + 2 * static_cast<int>(sync_feedback_.null_position) - static_cast<int>(mode_parameters_.null_size)) > 0 )
			{
				sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
				sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
			}

			if ( !demod_decod_created_ )
			{
				// Creating Demodulator, creating DataDecoder for decoding only FIC
				demodulator_ = new Demodulator(&mode_parameters_);
				datadecoder_ = new DataDecoder(&mode_parameters_);

				demod_decod_buffer_size_ = 2 * mode_parameters_.fic_size;
				demod_decod_buffer_ = new float[demod_decod_buffer_size_]();

				demod_read_write_.write_here = demod_decod_buffer_;
				decod_read_write_.read_here = demod_decod_buffer_;
				decod_read_write_.read_size = mode_parameters_.fic_size; // variable passed to DataDecoder but not used there.

				demod_decod_created_ = true;
			}

			if ( pointer_verbose_ )
			{
				FILE * pFile;
				pFile = fopen( "tmp/demod_pointer_debug", "w" );
				fprintf( pFile, "Buffer start address: %p, Buffer end address: %p\n", demod_decod_buffer_,  demod_decod_buffer_ + demod_decod_buffer_size_ );
				fprintf( pFile, "Demodulator::Process() (in CONF), counter: %zu, read address:  %p, write_address: %p\n", demod_counter_, demod_read_write_.read_here, demod_read_write_.write_here );
				fclose( pFile );
				demod_counter_++;
			}

			// Setting Demodulator read to end of null (FIC start)
			demod_read_write_.read_here = sync_read_.read_here + 3 * mode_parameters_.null_size;
			demod_read_position_ = sync_read_position_ + 3 * mode_parameters_.null_size;

			// Demodulate FIC
			demodulator_->Process( &station_info_, &demod_read_write_ );

			if ( pointer_verbose_ )
			{
				FILE * pFile;
				pFile = fopen( "tmp/decod_pointer_debug","w" );
				fprintf( pFile, "Datadecoder::Process() (in CONF), counter: %zu, read address:  %p\n", decod_counter_, decod_read_write_.read_here );
				fclose( pFile );
				decod_counter_++;
			}

			UserFICData_t *user_data = NULL;
			datadecoder_->Process( &decod_read_write_, &station_info_list_, &station_info_, user_data );
			delete user_data; //not needed at this step

			// If no station decoded in FIC, go to SYNC state
			std::list<stationInfo>::iterator it;
			if ( station_info_list_.size() > 0 )
			{
				station_info_list_.sort( StationsSort() );
				// Print out sorted list of station
				if ( verbose_ )
				{
					for (std::list<stationInfo>::iterator it=station_info_list_.begin(); it != station_info_list_.end(); ++it)
					{
						fprintf(stderr, "%d. Name: %s, channel start address: %zu, channel size: %zu, protection level: %d, protection level type: %d, is long: %d\n, serviceId: %d\n",
						it->SubChannelId, it->station_name.c_str(), it->sub_ch_start_addr, it->sub_ch_size, it->protection_level, it->ProtectionLevelTypeB, it->IsLong, it->ServiceId);
					}
				}

				/// Creating AudioDecoder, it->IsLong == 1 -> DAB+, it->IsLong == 0 -> DAB
				it=station_info_list_.begin();
				if( it->IsLong )
					audiodecoder_ = new AudioDecoder( 0.1, 50 * 10000 );
				else
					audiodecoder_ = new AudioDecoder( 0.1, 50 * 10000, PLAYER_MPEG );

				// Play on speakers
				if ( use_pulse_sink_ )
				{
					pulse_sink_ = new PulseSink();
					audiodecoder_->AddSink(pulse_sink_);
				}

				// Save to file
				if ( output_filename_ != NULL )
				{
					ogg_sink_ = new OggSink( output_filename_ );
					audiodecoder_->AddSink( ogg_sink_ );
				}

				audio_buffer_size_ = audio_buffer_multiplier_ * 3000;
				audio_buffer_ = new uint8_t[audio_buffer_size_]();
				audiodecoder_data.audio_decoder = audiodecoder_;
				audiodecoder_data.finish_work = false;

				// Create AudioProcess - blocking function, need to be on thread
				pthread_attr_init( &attr );
				pthread_create(&audiothread_, &attr, AudioProcess, &audiodecoder_data);

				// Decode station with number from -c option (default first station will be decoded)
				std::list<stationInfo>::iterator it;
				it=station_info_list_.begin();
				if (station_number_ == 255)
				{
					station_number_ = it->SubChannelId;
				}
				else
				{
					bool found = false;
					while (it != station_info_list_.end())
					{
						if (station_number_ == it->SubChannelId)
						{
							station_number_ = it->SubChannelId;
							found = true;
							break;
						}
						++it;
					}

					if (!found)
					{
						it=station_info_list_.begin();
						station_number_ = it->SubChannelId;
						///@todo maybe issue ParametersFromSDR with error
					}
				}

				// Calculate fs_drift on more frames
				number_of_frames_for_calculate_fsdrift_ = 10;

				state_ = CONFSTATION;

			}
			else
			{
				set_fs_drift_ = true;
				fs_drift_ = 0;
				fs_counter_ = 0;
				sync_frame_size_ = 0;
				state_ = SYNC;
			}

			sync_read_.read_here += sync_pointer_shift_size_;
			sync_read_position_ += sync_pointer_shift_size_;


		}
		else if ( sync_feedback_.null_quality == NULL_SHIFT )
		{
			if ( verbose_ )
				fprintf( stderr, "Null in conf_shift: %zu \n", sync_feedback_.null_position );

			// Reset calculating fs_drift, shift pointer, go to SYNC state
			set_fs_drift_ = true;
			fs_drift_ = 0;
			fs_counter_ = 0;
			sync_frame_size_ = 0;

			sync_read_.read_here = sync_read_.read_here + sync_feedback_.null_position - mode_parameters_.null_size + sync_pointer_shift_size_;
			sync_read_position_ = sync_read_position_ + sync_feedback_.null_position - mode_parameters_.null_size + sync_pointer_shift_size_;

			state_ = SYNC;
		}
		else if ( sync_feedback_.null_quality == NULL_NOT_DETECTED )
		{
			if ( verbose_ )
				fprintf( stderr, "******************* NULL not detected *************************\n" );

			// Reset calculating fs_drift, go to SYNC state
			set_fs_drift_ = true;
			fs_drift_ = 0;
			fs_counter_ = 0;
			sync_frame_size_ = 0;

			ParametersFromSDR(DAB_NOT_DETECTED);

			sync_read_.read_here += sync_pointer_shift_size_;
			sync_read_position_ += sync_pointer_shift_size_;

			state_ = SYNC;

		}
		else
		{
			if ( verbose_ )
				fprintf( stderr, "Error in Conf method, undefined null value" );
		}
	}

	// If mutex is locked, calculate new pointer position for writing, unlock mutex
	if(is_locked)
	{
		CalculateNewBufferPosition( ( data_write_.blocks_skipped + 1 ) * data_write_.block_size );
		data_write_.data_stored = false;

		if ( pthread_mutex_unlock( data_write_.lock_buffer ) )
		{
			if ( verbose_ )
				fprintf( stderr, "Can't unlock mutex in SYNC method (deadlock may occur)\n" );
		}
	}

	return state_;
}

Scheduler::state_t Scheduler::ConfStation()
{

	// Search for given station number from cli (station x option)
	std::list<stationInfo>::iterator it;

	for ( it=station_info_list_.begin(); it != station_info_list_.end(); ++it )
	{
		if(it->SubChannelId == station_number_)
		{
			station_found_ = true;
			break;
		}
		else if ( it == station_info_list_.end() )
		{
			it = station_info_list_.begin();
			station_found_ = false;
			break;
		}
	}

	// If found, change to it (delete DataDecoder and create it with proper parameters)
	if( station_found_ )
	{
		station_info_.SubChannelId = it->SubChannelId;
		station_info_.station_name = it->station_name;
		station_info_.sub_ch_start_addr = it->sub_ch_start_addr;
		station_info_.sub_ch_size = it->sub_ch_size;
		station_info_.protection_level = it->protection_level;
		station_info_.ProtectionLevelTypeB = it->ProtectionLevelTypeB;
		station_info_.IsLong= it->IsLong;
		station_info_.ServiceId = it->ServiceId;
		station_info_.audio_kbps = it->audio_kbps;
		station_found_ = false;

		delete datadecoder_;
		datadecoder_ = new DataDecoder(&station_info_, &mode_parameters_);

		// Delete demod_decod_buffer_ and create it with proper size
		CalculateDeqpskBuffer();

		// Set demod_write and decod_read pointers to proper position
		demod_read_write_.write_here = demod_decod_buffer_ + (mode_parameters_.number_of_deqpsk_unit_for_read - 1) * deqpsk_unit_size_;
		demod_write_position_ = (mode_parameters_.number_of_deqpsk_unit_for_read - 1) * deqpsk_unit_size_;

		decod_read_position_ = 0;
		decod_read_write_.read_here = demod_decod_buffer_;
		decod_read_write_.read_size = deqpsk_block_size_;
		decod_read_write_.write_here = audio_buffer_;
	}
	else
	{
		ParametersFromSDR(STATION_NOT_FOUND);
	}

	// Go to PLAY state
    state_ = PLAY;

    if ( verbose_ )
    	fprintf( stderr, "GOING TO PLAY STATE\n" );

    return state_;
}

Scheduler::state_t Scheduler::Play()
{
	bool is_locked = false;
	// If DataFeeder stored data, lock mutex, set pointer to end of wrote data.
	if(data_write_.data_stored)
	{
		pthread_mutex_lock( data_write_.lock_buffer );
		SetNewWritePointer();
		is_locked = true;
	}

	// When DataFeeder writes enough data to start processing frame
	if ( data_write_position_ > (sync_read_position_ + sync_read_size_) )
	{
		if ( pointer_verbose_ )
		{
			FILE * pFile;
			pFile = fopen( "tmp/sync_pointer_debug", "a" );
			fprintf( pFile, "Synchronizer::Process() (in PLAY), counter: %zu, read address:  %p, read + size: %p\n", sync_feeder_counter_, sync_read_.read_here, sync_read_.read_here + sync_read_size_ );
			fclose( pFile );
			sync_feeder_counter_++;
		}

		// Sync frame, detect null position and its quality, calcualte fc_drift
		synchronizer_->Process( sync_read_.read_here, mode_parameters_.sync_read_size, &sync_feedback_ );

		CalculateAndSetDrifts();

		// Process frame even when it may be not detected
		if ( sync_feedback_.null_quality == NULL_OK || sync_feedback_.null_quality == NULL_NOT_DETECTED)
		{

			if ( verbose_ && sync_feedback_.null_quality == NULL_OK )
				fprintf( stderr, "Null_ok: %zu \n", sync_feedback_.null_position );

			// Correct pointer position
			if(sync_feedback_.null_quality == NULL_OK)
			{
				if( ( static_cast<int>(sync_read_position_) + 2 * static_cast<int>(sync_feedback_.null_position) - static_cast<int>(mode_parameters_.null_size) ) > 0 )
				{
					sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
					sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size;
				}
			}

			// When null not detected, reset calculating fs_drift
			if ( sync_feedback_.null_quality == NULL_NOT_DETECTED )
			{
				ParametersToSDR(DAB_NOT_DETECTED);
				fs_drift_ = 0;
				fs_counter_ = 0;
				sync_frame_size_ = 0;
			}

			if ( pointer_verbose_ )
			{
				FILE * pFile;
				pFile = fopen( "tmp/demod_pointer_debug", "a" );
				fprintf( pFile, "Demodulator::Process() (in PLAY), counter: %zu, read address:  %p, write_address: %p\n", demod_counter_, demod_read_write_.read_here, demod_read_write_.write_here );
				fclose( pFile );
				demod_counter_++;
			}

			// Setting Demodulator read to end of null (FIC start)
			demod_read_write_.read_here = sync_read_.read_here + 3 * mode_parameters_.null_size; // demodulator reads at end of null
			demod_read_position_ = sync_read_position_ + 3 * mode_parameters_.null_size;

			// Demodulate FIC and MSC of given station
			demodulator_->Process( &station_info_, &demod_read_write_ );

			sync_read_.read_here += sync_pointer_shift_size_;
			sync_read_position_ += sync_pointer_shift_size_;

			if ( pointer_verbose_ )
			{
				FILE * pFile;
				pFile = fopen( "tmp/decod_pointer_debug", "a" );
				fprintf( pFile, "Datadecoder::Process() (in PLAY), counter: %zu, read address:  %p, read_size: %p\n", decod_counter_, decod_read_write_.read_here, decod_read_write_.read_here + deqpsk_block_size_ );
				fclose( pFile );
				decod_counter_++;
			}

			// Decode FIC and MSC of given station
			UserFICData_t *user_data = NULL;
			datadecoder_->Process( &decod_read_write_, &station_info_list_, &station_info_, user_data );
			ParametersFromSDR( user_data );

			station_info_list_.sort( StationsSort() );
			if ( 0 )
			{
				// Print out sorted list of station (turned off for now)
				for (std::list<stationInfo>::iterator it=station_info_list_.begin(); it != station_info_list_.end(); ++it)
				{
					fprintf(stderr, "%d. Name: %s, channel start address: %zu, channel size: %zu, protection level: %d, protection level type: %d, is long: %d\n, serviceId: %d\n",
					it->SubChannelId, it->station_name.c_str(), it->sub_ch_start_addr, it->sub_ch_size, it->protection_level, it->ProtectionLevelTypeB, it->IsLong, it->ServiceId);
				}
			}

			// Setting Demodulator write and DataDecoder pointers to proper position
			decod_read_write_.read_here += deqpsk_unit_size_;
			decod_read_position_ += deqpsk_unit_size_;
			demod_read_write_.write_here += deqpsk_unit_size_;
			demod_write_position_ += deqpsk_unit_size_;

			// Copy data if needed (Datadecoder must have continous data for read)
			if(demod_write_position_ + deqpsk_unit_size_ > demod_decod_buffer_size_)
			{
				memcpy(demod_decod_buffer_, demod_decod_buffer_ + demod_decod_buffer_size_ - deqpsk_block_size_ + deqpsk_unit_size_,
						sizeof(float) * (deqpsk_block_size_ - deqpsk_unit_size_));
				demod_read_write_.write_here -= decod_read_position_ ;
				demod_write_position_ -= decod_read_position_ ;
				decod_read_write_.read_here -= decod_read_position_;
				decod_read_position_ = 0;
			}

			if ( verbose_ )
				fprintf(stderr, "Audio write size: %zu \n", decod_read_write_.write_size);

			// Write decoded music to AudioDecoder Process() method in  AudioProcess() thread
			if( decod_read_write_.write_size > 0 )
				audiodecoder_->Write(audio_buffer_,decod_read_write_.write_size);


		}
		else if ( sync_feedback_.null_quality == NULL_SHIFT )
		{
			if ( verbose_ )
				fprintf( stderr, "Null_shift: %zu \n", sync_feedback_.null_position );

			// Reset calculating fs_drift, shift pointer.
			fs_drift_ = 0;
			fs_counter_ = 0;
			sync_frame_size_ = 0;

			sync_read_.read_here = sync_read_.read_here + 2 * sync_feedback_.null_position - mode_parameters_.null_size + sync_pointer_shift_size_;
			sync_read_position_ = sync_read_position_ + 2 * sync_feedback_.null_position - mode_parameters_.null_size + sync_pointer_shift_size_;

		}
	}

	// If mutex is locked, calculate new pointer position for writing, unlock mutex
	if(is_locked)
	{
		CalculateNewBufferPosition( ( data_write_.blocks_skipped + 1 ) * data_write_.block_size );
		data_write_.data_stored = false;

		if ( pthread_mutex_unlock( data_write_.lock_buffer ) )
		{
			if ( verbose_ )
				fprintf( stderr, "Can't unlock mutex in SYNC method (deadlock may occur)\n" );
		}
	}

	return state_;

}

void Scheduler::CalculateDeqpskBuffer(){

	delete [] demod_decod_buffer_;
	deqpsk_unit_size_ = 64 * (mode_parameters_.number_of_symbols_per_fic +
			((station_info_.sub_ch_start_addr + station_info_.sub_ch_size - 1) / mode_parameters_.number_cu_per_symbol -
			 station_info_.sub_ch_start_addr / mode_parameters_.number_cu_per_symbol + 1) * mode_parameters_.number_of_cif) *
			mode_parameters_.number_cu_per_symbol;

	deqpsk_block_size_ = deqpsk_unit_size_ * mode_parameters_.number_of_deqpsk_unit_for_read;
	demod_decod_buffer_size_ = deqpsk_block_size_ * demod_decod_buffer_multiplier_;

	demod_decod_buffer_ = new float[demod_decod_buffer_size_]();

	if ( pointer_verbose_ )
	{
		FILE * pFile;
		pFile = fopen( "tmp/demod_pointer_debug", "a" );
		fprintf( pFile, "NEW demod_decod_buffer_\n" );
		fprintf( pFile, "WRITE Buffer start address: %p, Buffer end address: %p\n", demod_decod_buffer_,  demod_decod_buffer_ + demod_decod_buffer_size_ );
		fprintf( pFile, "READ Buffer start address: %p, Buffer end address: %p\n", sync_feeder_buffer_,  sync_feeder_buffer_ + sync_feeder_buffer_size_ );
		fclose( pFile );
	}

}

void Scheduler::CalculateAndSetDrifts() {

	if ( fs_counter_ >=  number_of_frames_for_calculate_fsdrift_ + 2 )
	{
		// fs_drift in ppm (parts per milion)
		fs_drift_ = -((static_cast<int>(mode_parameters_.frame_size) - static_cast<float>(sync_frame_size_) / (number_of_frames_for_calculate_fsdrift_ * 2)) * 1000000) / mode_parameters_.frame_size;

		if ( verbose_ )
			fprintf( stderr, "Fs drift: %f, Fc drift: %f \n", fs_drift_, sync_feedback_.fc_drift );

		// Set fs_drift if it is greater than 2.7 or lower than -2.7
		if (fs_drift_ < -2.7 || fs_drift_ > 2.7)
		{
			set_fs_drift_ = true;
			if( state_ == PLAY && sqrt((previous_fc_drift - sync_feedback_.fc_drift) * (previous_fc_drift - sync_feedback_.fc_drift)) < 5 )
			{
				datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift,  fs_drift_ );
			}
			else if (state_ != PLAY)
			{
				datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift,  fs_drift_ );
			}
			else
			{
				datafeeder_->HandleDrifts( 0, fs_drift_ );
			}
		}
		else
		{
			// Set fc_drift. It is returned in kHz, but must be set in Hz
			if( state_ == PLAY && sqrt((previous_fc_drift - sync_feedback_.fc_drift) * (previous_fc_drift - sync_feedback_.fc_drift)) < 5 )
			{
				datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift, 0 );
			}
			else if (state_ != PLAY)
			{
				datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift, 0 );
			}
			set_fs_drift_ = false;
		}

		fs_drift_ = 0;
		fs_counter_ = 0;
		sync_frame_size_ = 0;

	}
	else if ( fs_counter_ > 1 )
	{
		previous_null_ = current_null_;
		current_null_ = sync_read_position_ + sync_feedback_.null_position;

		if ( current_null_ > previous_null_ )
		{
			sync_frame_size_ += (current_null_ - previous_null_);
		}
		else if ( current_null_ < previous_null_ )
		{
			sync_frame_size_ += (sync_feeder_buffer_end_data_ - previous_null_ + current_null_);
		}
		else
		{
			if ( verbose_ )
				fprintf( stderr, "Error in CalculateAndSetDrifts when calculating sync_frame_size_" );
		}

		if ( verbose_ )
			fprintf( stderr,"Fc drift: %f \n", sync_feedback_.fc_drift );
		// Set fc_drift. It is returned in kHz, but must be set in Hz
		if( state_ == PLAY && sqrt((previous_fc_drift - sync_feedback_.fc_drift) * (previous_fc_drift - sync_feedback_.fc_drift)) < 5 )
		{
			datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift, 0 );
		}
		else if (state_ != PLAY)
		{
			datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift, 0 );
		}


	}
	else if ( fs_counter_ == 1 )
	{
		current_null_ = sync_read_position_ + sync_feedback_.null_position;

		if ( verbose_ )
			fprintf( stderr,"Fc drift: %f \n", sync_feedback_.fc_drift );

		// Set fc_drift. It is returned in kHz, but must be set in Hz
		if( state_ == PLAY && sqrt((previous_fc_drift - sync_feedback_.fc_drift) * (previous_fc_drift - sync_feedback_.fc_drift)) < 5 )
		{
			datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift, 0 );
		}
		else if (state_ != PLAY)
		{
			datafeeder_->HandleDrifts( -500 * sync_feedback_.fc_drift, 0 );
		}
	}
	previous_fc_drift = sync_feedback_.fc_drift;
	fs_counter_++;

}

void Scheduler::SetNewWritePointer()
{
	if ( data_write_.data_stored )
	{

		// If blocks are skipped
		if ( data_write_.blocks_skipped )
		{
			if ( verbose_ )
				fprintf( stderr, "Blocks skipped: %zu, lost data: %zu \n", data_write_.blocks_skipped, data_write_.blocks_skipped * data_write_.block_size );

			// More skipped should be ignored, there will be too much data lost
			if ( data_write_.blocks_skipped % 2 )
				data_write_.blocks_skipped = 1;
			else
				data_write_.blocks_skipped = 0;

			// If data_write_position_ + offset is lower than buffer_size
			if ( data_write_position_ + ( ( data_write_.blocks_skipped + 1 ) * data_write_.block_size ) <= sync_feeder_buffer_size_ )
			{
				float j = 0.3;
				//Memset with -j and j
				for ( size_t i = 0; i < (data_write_.blocks_skipped * data_write_.block_size); i++ )
				{
					memset( sync_feeder_buffer_ + data_write_position_ + data_write_.block_size, j, sizeof(float) );
					j = -j;
				}
			}
			// If data_write_position_ + offset is greater than buffer_size
			else if ( data_write_position_ + ( ( data_write_.blocks_skipped + 1 ) * data_write_.block_size ) > sync_feeder_buffer_size_ )
			{
				float j = 0.3;
				// Memset with -j and j
				for ( size_t i = 0; i < (sync_feeder_buffer_size_ - data_write_position_ - data_write_.block_size); i++ )
				{
					memset( sync_feeder_buffer_ + data_write_position_ + data_write_.block_size, j, sizeof(float) );
					j = -j;
				}

			}



			// Reset calculating fs_drift
			fs_drift_ = 0;
			fs_counter_ = 0;
			sync_frame_size_ = 0;

		}

		// Set proper write pointer position
		data_write_.write_here += (( data_write_.blocks_skipped + 1 ) * data_write_.block_size);
		data_write_position_ += (( data_write_.blocks_skipped + 1 ) * data_write_.block_size);

		data_write_.blocks_skipped = 0;
	}
}


void Scheduler::CalculateNewBufferPosition(size_t offset)
{
	// DATA WRITE FILLS
	if(data_write_position_ + offset <= sync_feeder_buffer_size_ )
	{
		// SYNC READ NOT FILLS
		if( ( sync_read_position_ + sync_read_size_ > sync_feeder_buffer_size_ ) && ( data_write_position_ > sync_read_position_ ) )
		{

			if ( verbose_ )
				fprintf( stderr, "DATA_WRITE FILLS AND SYNC_READ NOT FILLS (MEMCPY), COPIED: %zu \n",data_write_position_ - sync_read_position_ );

			sync_feeder_buffer_end_data_ = sync_read_position_;

			memcpy(sync_feeder_buffer_, sync_feeder_buffer_ + (sync_read_position_)
					, sizeof(float) * (data_write_position_ - sync_read_position_ ));

			data_write_.write_here -= sync_read_position_;
			data_write_position_ -= sync_read_position_;
			sync_read_.read_here -= sync_read_position_;
			sync_read_position_ = 0;
		}

	}
	// DATA WRITE NOT FILLS ( DATA WRITE POS < SYNC_FEEDER_BUFFER_SIZE_)
	else if ( ( data_write_position_ + offset > sync_feeder_buffer_size_ ) && ( data_write_position_ <= sync_feeder_buffer_size_ ) )
	{
	// WRITE_POS + WRITE_SIZE = NOT FILLS

		if ( ( sync_read_position_ + sync_read_size_ > data_write_position_ ) && ( data_write_position_ > sync_read_position_ ) )
		{

			// FIRST CONDITION
			// SYNC_POS + SYNC_SIZE = FILLS IN BUFFER AND WRITE_POS + WRITE_SIZE = NOT FILLS
			// SYNC_POS + SYNC_SIZE > WRITE_POS
			// !COPY SIZE! = WRITE_POS - SYNC_POS

			// SECOND CONDITION
			// SYNC_POS + SYNC_SIZE = NOT FILLS IN BUFFER AND WRITE_POS + WRITE_SIZE = NOT FILLS
			// SYNC_POS < WRITE_POS
			// !COPY SIZE! = WRITE_POS - SYNC_POS


			if( verbose_ )
			{
				if( sync_read_position_ + sync_read_size_ < sync_feeder_buffer_size_ )
					fprintf( stderr, "DATA_WRITE NOT FILLS AND SYNC_READ FILLS (MEMCPY), COPIED: %zu \n", data_write_position_ - sync_read_position_ );
				if( sync_read_position_ + sync_read_size_ > sync_feeder_buffer_size_ )
					fprintf( stderr, "DATA_WRITE NOT FILLS AND SYNC_READ NOT FILLS (MEMCPY), COPIED: %zu \n", data_write_position_ - sync_read_position_ );
			}
			sync_feeder_buffer_end_data_ = sync_read_position_;

			memcpy(sync_feeder_buffer_, sync_feeder_buffer_ + sync_read_position_
					, sizeof(float) * ( data_write_position_ - sync_read_position_ ));

			data_write_.write_here -= sync_read_position_;
		    data_write_position_ -= sync_read_position_;
			sync_read_.read_here -= sync_read_position_;
			sync_read_position_ = 0;


		}
		else if ( ( sync_read_position_ + sync_read_size_ > sync_feeder_buffer_size_ ) && (sync_read_position_ >= data_write_position_))
		{
			// SYNC + SIZE = NOT FILLS IN BUFFER AND WRITE + SIZE = NOT FILLS
			// SYNC POS > WRITE POS
			// UNDO EVERYTHING TO START OF A BUFFER
			if ( verbose_ )
				fprintf(stderr, "DATA_WRITE NOT FILLS AND SYNC_READ NOT FILLS (SYNC_READ_POS > DATA_WRITE_POS) (UNDO POINTERS)\n");

			sync_feeder_buffer_end_data_ = sync_read_position_;
			sync_read_.read_here -= data_write_position_;
			sync_read_position_ -= data_write_position_;
			data_write_.write_here -= data_write_position_;
			data_write_position_ = 0;

		}
		else if ( sync_read_position_ + sync_read_size_ < data_write_position_ )
		{
			// SYNC_POS + SYNC_SIZE = FILLS IN BUFFER AND WRITE_POS + WRITE_SIZE = NOT FILLS
			// SYNC_POS + SYNC_SIZE < WRITE_POS

			memcpy( sync_feeder_buffer_, sync_feeder_buffer_ + sync_read_position_
					, sizeof(float) * ( data_write_position_ - sync_read_position_ ) );

			if ( verbose_ )
				fprintf( stderr, "MORE COPIED DATA, COPIED: %zu, CONTINUE WORKING \n", data_write_position_ - sync_read_position_ );

			data_write_.write_here -= sync_read_position_;
			data_write_position_ -= sync_read_position_;
			sync_read_.read_here -= sync_read_position_;
			sync_read_position_ = 0;

			fs_drift_ = 0;
			fs_counter_ = 0;
			sync_frame_size_ = 0;
		}

	}
	else if ( data_write_position_ >= sync_feeder_buffer_size_ )
	{
		// WHEN data_write_position_ before adding offset is bigger than sync_feeder_buffer_size_
		// I think it may occur when skipping block at the end of buffer

			if ( verbose_ )
				fprintf( stderr, "DATA WRITE NOT FILLS (data_write_position > sync_feeder_buffer_size), COPIED: %zu\n", sync_feeder_buffer_size_ - sync_read_position_ );

			sync_feeder_buffer_end_data_ = sync_read_position_;

			memcpy( sync_feeder_buffer_, sync_feeder_buffer_ + sync_read_position_
					, sizeof(float) * ( sync_feeder_buffer_size_ - sync_read_position_ ) );

			data_write_.write_here -= sync_read_position_;
			data_write_position_ -= sync_read_position_;
			sync_read_.read_here -= sync_read_position_;
			sync_read_position_ = 0;

	}
	else
	{
		if ( verbose_ )
		{
			fprintf(stderr, "Error in CalculateNewBuffer function, probably forgot about some case\n");
			fprintf(stderr, "sync_read_position: %zu\n", sync_read_position_);
			fprintf(stderr, "sync_read_position + sync_read_size: %zu\n", sync_read_position_ + sync_read_size_);
			fprintf(stderr, "bloks skipped: %zu\n", data_write_.blocks_skipped);
			fprintf(stderr, "data_write_position: %zu\n", data_write_position_);
			fprintf(stderr, "data_write_position + offset: %zu\n", data_write_position_ + offset);
			fprintf(stderr, "buffer size: %zu\n", sync_feeder_buffer_size_);
		}
		Scheduler::Stop(ERROR_UNKNOWN);
	}

}


int Scheduler::SetModeParameters(transmissionMode mode)
{
	switch ( mode )
	{
		case DAB_MODE_I:
		{
			mode_parameters_.guard_size = 504;
			mode_parameters_.fft_size = 2048;
			mode_parameters_.symbol_size = 2552;
			mode_parameters_.number_of_symbols = 76;
			mode_parameters_.null_size = 2656;
			mode_parameters_.frame_size = 196608;
			mode_parameters_.number_of_carriers = 1536;
			mode_parameters_.number_of_symbols_per_fic = 3;
			mode_parameters_.number_of_fib = 12;
			mode_parameters_.number_of_cif = 4;
			mode_parameters_.number_of_deqpsk_unit_for_read = 5;
			mode_parameters_.number_of_fib_per_cif = 3;
			mode_parameters_.number_samp_after_timedep = 3096;
			mode_parameters_.number_samp_after_vit = 768;
			mode_parameters_.sync_read_size = 199264;
			mode_parameters_.fic_size = 9216;
			mode_parameters_.number_cu_per_symbol = 48;
			mode_parameters_.number_symbols_per_cif = 18;
			mode_parameters_.dab_mode = DAB_MODE_I;
			break;
		}
		case DAB_MODE_II:
		{
			mode_parameters_.guard_size = 126;
			mode_parameters_.fft_size = 512;
			mode_parameters_.symbol_size = 638;
			mode_parameters_.number_of_symbols = 76;
			mode_parameters_.null_size = 664;
			mode_parameters_.frame_size = 49152;
			mode_parameters_.number_of_carriers = 384;
			mode_parameters_.number_of_symbols_per_fic = 3;
			mode_parameters_.number_of_fib = 3;
			mode_parameters_.number_of_cif = 1;
			mode_parameters_.number_of_deqpsk_unit_for_read = 17;
			mode_parameters_.number_of_fib_per_cif = 3;
			mode_parameters_.number_samp_after_timedep = 3096;
			mode_parameters_.number_samp_after_vit = 768;
			mode_parameters_.sync_read_size = 49816;
			mode_parameters_.fic_size = 2304;
			mode_parameters_.number_cu_per_symbol = 12;
			mode_parameters_.number_symbols_per_cif = 72;
			mode_parameters_.dab_mode = DAB_MODE_II;
			break;
		}
		case DAB_MODE_III:
		{
			mode_parameters_.guard_size = 63;
			mode_parameters_.fft_size = 256;
			mode_parameters_.symbol_size = 319;
			mode_parameters_.number_of_symbols = 153;
			mode_parameters_.null_size = 345;
			mode_parameters_.frame_size = 49152;
			mode_parameters_.number_of_carriers = 192;
			mode_parameters_.number_of_symbols_per_fic = 8;
			mode_parameters_.number_of_fib = 4;
			mode_parameters_.number_of_cif = 1;
			mode_parameters_.number_of_deqpsk_unit_for_read = 17;
			mode_parameters_.number_of_fib_per_cif = 4;
			mode_parameters_.number_samp_after_timedep = 4120;
			mode_parameters_.number_samp_after_vit = 1024;
			mode_parameters_.sync_read_size = 49497;
			mode_parameters_.fic_size = 3072;
			mode_parameters_.number_cu_per_symbol = 6;
			mode_parameters_.number_symbols_per_cif = 144;
			mode_parameters_.dab_mode = DAB_MODE_III;
			break;
		}
		case DAB_MODE_IV:
		{
			mode_parameters_.guard_size = 252;
			mode_parameters_.fft_size = 1024;
			mode_parameters_.symbol_size = 1276;
			mode_parameters_.number_of_symbols = 76;
			mode_parameters_.null_size = 1328;
			mode_parameters_.frame_size = 98304;
			mode_parameters_.number_of_carriers = 768;
			mode_parameters_.number_of_symbols_per_fic = 3;
			mode_parameters_.number_of_fib = 6;
			mode_parameters_.number_of_cif = 2;
			mode_parameters_.number_of_fib_per_cif = 3;
			mode_parameters_.number_of_deqpsk_unit_for_read = 9;
			mode_parameters_.number_samp_after_timedep = 3096;
			mode_parameters_.number_samp_after_vit = 768;
			mode_parameters_.sync_read_size = 99632;
			mode_parameters_.fic_size = 4608;
			mode_parameters_.number_cu_per_symbol = 24;
			mode_parameters_.number_symbols_per_cif = 36;
			mode_parameters_.dab_mode = DAB_MODE_IV;
			break;
		}
		case DAB_MODE_UNKNOWN:
		default:
		{
			if ( verbose_ )
				fprintf( stderr, "Error in SetModeParameters function (trying to set parameters for DAB_MODE_UNKNOWN or unrecognized enum value" );
			return 1;
		}
	}
	return 0;
}

void Scheduler::Process( data_source_t data_source )
{
	// Start state machine
	while ( true )
	{
		// Check if DataFeeder is running (it stops when file ends)
		if ( !datafeeder_->IsRunning() )
			program_end_delay_++;

		// If everything works
		if ( program_end_delay_ < 2 )
		{

			// If user stops program, go ti EXTERNAL_STOP state
			if ( requested_stop_ )
				state_ = EXTERNAL_STOP;

			// If user change station go to CONFSTATION state
			if (station_number_ != station_info_.SubChannelId && state_ == PLAY)
			{
				state_ = CONFSTATION;
				audiodecoder_->Flush();
			}

			switch ( state_ )
			{
				case SYNC:
					state_ = Sync( data_source );
					break;

				case CONF:
					state_ = Conf();
					break;

				case CONFSTATION:
					state_ = ConfStation();
					break;

				case PLAY:
					state_ = Play();
					break;

				case EXTERNAL_STOP:
					program_end_delay_ = 2;
					if(verbose_)
						fprintf(stderr, "External stop has been requested. \n");
					break;

				case INTERNAL_ERROR:
					program_end_delay_ = 2;
					if(verbose_)
						fprintf(stderr, "Error in Process function: internal error occurs. \n");
					break;

				default:
					if(verbose_)
						fprintf( stderr, "Error in Process function: machine state enters default case. \n" );
			}
		}
		// If data processing stops
		else
		{

			if ( state_ == SYNC || state_ == CONF || state_ == CONFSTATION || state_ == PLAY )
			{
				if ( data_source == DATA_FROM_DONGLE )
					Stop(DEVICE_DISCONNECTED);
				else if ( data_source == DATA_FROM_FILE )
					Stop(FILE_END);
			}
			else if( state_ == EXTERNAL_STOP )
			{
				Stop(OK);
			}
			else
			{
				Stop(ERROR_UNKNOWN);
			}

			if(verbose_)
				fprintf( stderr, "Dongle finished work\n");

			break;

		}
	}
}

void Scheduler::ListDevices(std::list<std::string> *list)
{
	static_cast<DataFeeder*>(datafeeder_)->GetDongleList(list);
}


void Scheduler::Start(SchedulerConfig_t config)
{
	//set audio properties
	use_pulse_sink_ = config.use_speakers;
	output_filename_ = config.output_filename;

	const char * dongle_or_file_name;
	if (config.data_source == DATA_FROM_DONGLE)
	{
		char dongle_name[10]; //DataFeeder expects c-string
		snprintf(dongle_name, 10, "%zu", config.dongle_nr);
		dongle_or_file_name = dongle_name;
	}
	else
	{
		dongle_or_file_name = config.input_filename;
	}

	station_number_ = config.start_station_nr;

	state_ = Init(dongle_or_file_name, 4, 12 * 16384, config.sampling_rate, config.carrier_frequency, config.data_source);

	if(state_ == SYNC)
	{
		Process(config.data_source);
	}
	else if (state_ == INIT_ERROR)
	{
		ParametersFromSDR(config.data_source == DATA_FROM_DONGLE ? DEVICE_NOT_FOUND : FILE_NOT_FOUND);
	}
	else if (state_ == INTERNAL_ERROR)
	{
		ParametersFromSDR(ERROR_UNKNOWN);
	}
}

void Scheduler::Stop(void) {
	requested_stop_ = true; //should be thread safe
}

void Scheduler::Stop(scheduler_error_t error_code)
{

	data_write_.finish_rtl_process = true;
	//while(datafeeder_->IsRunning());
	pthread_join(datafeeder_startprocessing_thread_, NULL);

	if (audiodecoder_ != NULL)
	{
		audiodecoder_data.finish_work = true;

		if( error_code == FILE_END)
			audiodecoder_->LastFrame();
		else
			audiodecoder_->Flush();
	}

	pthread_join(audiothread_, NULL);
	ParametersFromSDR(error_code);
}

void Scheduler::ParametersFromSDR(scheduler_error_t error_code)
{
	return; //no need to implement further
}

void Scheduler::ParametersFromSDR(float snr)
{
	return; //no need to implement further
}

void Scheduler::ParametersFromSDR(UserFICData_t *user_fic_extra_data)
{
	delete user_fic_extra_data; //no need to implement further
}

void Scheduler::ParametersFromSDR(std::string *text)
{
	delete text; //no need to implement further
}

void Scheduler::ParametersToSDR(uint8_t new_station)
{
	station_number_ = new_station;
}

void Scheduler::VerbosityOn()
{
	verbose_ = true;
}

Scheduler::~Scheduler()
{
	delete [] audio_buffer_;
	delete audiodecoder_;
	delete datadecoder_;
	delete [] demod_decod_buffer_;
	delete demodulator_;
	delete [] sync_feeder_buffer_;
	delete synchronizer_;
	delete datafeeder_;
}


