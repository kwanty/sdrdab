/**
 * @file cli_scheduler.cc
 * @brief CLIScheduler implementation
 *
 * @author Krzysztof Szczęsny, kaszczesny@gmail.com
 * @date Created on: 3 June 2015
 * @version 1.0
 * @copyright Copyright (c) 2015 Krzysztof Szczęsny
 * @pre sdrdab + librtlsdr (dynamic linkage)
 * @pre libreadline6
 * @par License
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see \<http://www.gnu.org/licenses/\>.
 */

#include "cli_scheduler.h"
#include <pthread.h>
#include <unistd.h> //isatty
/// @cond
#include <cstdlib>
/// @endcond

volatile CLIScheduler::scheduler_error_t CLIScheduler::errno_ = OK;

/// set the array of available commands for ReadLine
const ReadLine::command_t ReadLine::command_vector[] = {
	{ "station", CLIScheduler::COMMAND_STATION, "Change current station." },
	{ "list",    CLIScheduler::COMMAND_LIST,    "Display list of available stations." },
	{ "quit",    CLIScheduler::COMMAND_QUIT,    "Exit program." },
	/*
	{ "mute",    CLIScheduler::COMMAND_MUTE,    "Mute sound." },
	{ "volume",  CLIScheduler::COMMAND_VOLUME,  "Get/set sound volume to desired level (0-100)." },
	{ "image",   CLIScheduler::COMMAND_IMAGE,   "Get information about current and next SlideShow image." },
	*/
	{ "help",    CLIScheduler::COMMAND_HELP,    "Display this help message." },
	{ "restart", CLIScheduler::COMMAND_RESTART, "Restart program." },
	{ NULL,      0,                             NULL }
};

CLIScheduler::CLIScheduler(int argc, char **argv)
	: Scheduler(),
	  user_input_(new UserInput()),
	  scheduler_thread_(0),
	  scheduler_thread_running_(true),
	  user_data_(),
	  user_data_mutex_() {
	pthread_mutex_init(&this->user_data_mutex_, NULL);

	if (argc > 0) {
		this->user_input_->Process(argc, argv);

		if (this->user_input_->verbose_)
			this->VerbosityOn();
	}

}

CLIScheduler::~CLIScheduler() {
	delete this->user_input_;
	pthread_mutex_destroy(&this->user_data_mutex_);
}

CLIScheduler *CLIScheduler::Revive(void) {
	CLIScheduler *new_cli = new CLIScheduler(0, NULL);

	new_cli->user_input_ = this->user_input_;
	if (new_cli->user_input_->verbose_)
		new_cli->VerbosityOn();
	this->user_input_ = NULL; //user_input_ belongs now to new instance
	delete this;
	CLIScheduler::errno_ = OK; //just in case
	return new_cli;
}

void CLIScheduler::ParametersFromSDR(scheduler_error_t error_code) {
	switch (error_code) {
	  case OK:
		CLIScheduler::errno_ = OK;
		break;

	  case DAB_NOT_DETECTED:
		ReadLine::printf("DAB signal was not detected!\n");
		break;

	  case STATION_NOT_FOUND:
		ReadLine::printf("  Unknown station.\n  Type 'list' to get list of "
						 "stations available on current multiplex.\n");
		break;

	  default:
		CLIScheduler::errno_ = error_code;
		this->PrintErrorMsg();
		ReadLine::printf("Press Enter to exit.\n");
	}
}

void CLIScheduler::ParametersFromSDR(float snr) {
	if (snr != snr) //NaN
		ReadLine::printf("SNR measurement has failed.\n");
	else
		ReadLine::printf("SNR: %.1g dB\n", snr);
}

void CLIScheduler::ParametersFromSDR(UserFICData_t *user_fic_extra_data) {

	pthread_mutex_lock(&this->user_data_mutex_);
	this->user_data_.Set(user_fic_extra_data);
	pthread_mutex_unlock(&this->user_data_mutex_);

	delete user_fic_extra_data;
}

void CLIScheduler::ParametersFromSDR(std::string *text) {
	size_t len = text->length();
	if (len > 0) {
		if ((*text)[len - 1] != '\n')
			text->append("\n");
		ReadLine::printf("%s", text->c_str());
	}
	delete text;
}

bool CLIScheduler::Process(void) {
	char *line;
	ReadLine::parsed_command_t command;
	bool quit = false, restart = false;

	/* first below condition means that loop will generally stop after
	 * Scheduler's thread calls ParametersFromSDR(scheduler_error_t) */
	while (CLIScheduler::errno_ == OK && !quit && !restart) {
		line = readline(ReadLine::prompt_);

		if (line == NULL) {
			CLIScheduler::errno_ = ERROR_UNKNOWN;
			quit = true;
			break;
		}

		if (this->user_input_->info_) {
			pthread_mutex_lock(&this->user_data_mutex_);
			this->user_data_.Print();
			pthread_mutex_unlock(&this->user_data_mutex_);
		}

		if (line[0] != '\0') {
		  command = ReadLine::ParseLine(line);

		  if (strcmp(line, "?") == 0)
			  command.command_id_ = COMMAND_HELP;

		  switch(command.command_id_) {
		    case COMMAND_STATION:
		      pthread_mutex_lock(&this->user_data_mutex_);
		      if (this->user_data_.stations.empty())
		    	  ReadLine::printf("Station list is being populated. Please retry in a moment.\n");
		      else
		    	  this->CommandStation(command.arg_);
			  pthread_mutex_unlock(&this->user_data_mutex_);
		      break;

		    case COMMAND_LIST:
		      pthread_mutex_lock(&this->user_data_mutex_);
		      if (this->user_data_.stations.empty())
		    	  ReadLine::printf("Station list is empty.\n");
		      else
		    	  this->CommandList();
		      pthread_mutex_unlock(&this->user_data_mutex_);
		      break;

		    case COMMAND_QUIT:
		      quit = true;
		      break;

		    case COMMAND_RESTART:
		      restart = true;
		      break;

		    case COMMAND_MUTE:
		      ///@todo mute (ParametersToSDR(???))
		      ReadLine::printf("WARNING: audio muting hasn't been implemented.\n");
		      break;

		    case COMMAND_VOLUME:
		      if (command.arg_ == NULL || command.arg_[0] == '\0') { //get
		    	  ///@todo volume (ParametersToSDR(???) - getter)
		    	  ReadLine::printf("WARNING: getting current volume hasn't been implemented.\n");
		      } else { //set
		    	  unsigned short vol;
		    	  if (sscanf(command.arg_, "%hu", &vol) != 1 ||
		    		  vol > 100)
		    	  {
		    		  ReadLine::printf("  Incorrect volume level given. Please "
		    				  	  	     "use a number (0-100), i.e.: 'volume 30'\n");
		    	  } else {
		    		  ///@todo volume (ParametersToSDR(???) - setter)
		    		  ReadLine::printf("WARNING: setting volume hasn't been implemented.\n");
		    	  }
		      }
		      break;

		    case COMMAND_IMAGE:
		      ///@todo image (get information stored in user_data_)
			  ReadLine::printf("WARNING: this feature hasn't been implemented.\n");
		      break;

		    case COMMAND_HELP:
		      {
				  const ReadLine::command_t *command_ptr = ReadLine::command_vector;
				  ReadLine::printf("\tCommand:Description:\n");
				  while (command_ptr->name_ != NULL) {
					  ReadLine::printf("\t%s\t%s\n", command_ptr->name_,
									   command_ptr->doc_);
					  ++command_ptr;
				  }
		      }
		      break;

		    case COMMAND_UNKNOWN: //just to be very explicit
		    default:
		      ReadLine::printf("  Unknown or malformed command was issued. "
		    		             "Type 'help' and press Enter for help.\n");
		      break;
		  }
		}

		free(line);
	}

	this->Stop(); //issue stop "signal"
	if (restart)
		ReadLine::printf("Restarting...\n");
	else if (this->scheduler_thread_running_)
		ReadLine::printf("NOTICE: Waiting for sdrdab to terminate its operation.\n");
	pthread_join(this->scheduler_thread_, NULL);

	return restart;
}

void CLIScheduler::FireSchedulerThread() {
	if( 0!= pthread_create(&this->scheduler_thread_,
						   NULL,
						   CLIScheduler::Start,
						   static_cast<void *>(this)) )
	{
		ReadLine::printf("FATAL ERROR: Thread creation failed.");
		delete this;
		std::exit(CLIScheduler::PTHREAD_CREATE_FAIL);
	}
}

void *CLIScheduler::Start(void *this_ptr) {
	CLIScheduler *this_ = static_cast<CLIScheduler *>(this_ptr);

	SchedulerConfig_t config; //invokes default SchedulerConfig_t constructor
	config.sampling_rate  = this_->user_input_->sampling_rate_;
	config.carrier_frequency = this_->user_input_->freq_;
	if (this_->user_input_->FromTuner())
		config.dongle_nr = static_cast<size_t>(this_->user_input_->tuner_);
	else {
		config.input_filename = this_->user_input_->file_;
		config.data_source = Scheduler::DATA_FROM_FILE;
	}
	config.use_speakers = !(this_->user_input_->silent_);
	config.output_filename = this_->user_input_->output_;
	if (this_->user_input_->channel_nr > 0)
		config.start_station_nr = this_->user_input_->channel_nr;

	this_->Scheduler::Start(config);

	this_->scheduler_thread_running_ = false;
	return NULL;
}

void CLIScheduler::RequestedList(void) {
	if (this->user_input_->list_) {
		std::list<std::string> devices;
		this->ListDevices( &devices );

		if (devices.empty()) {
			printf("No compatible devices have been found.\n");
		} else {
			printf("Compatible devices:\n");
			std::list<std::string>::iterator it;
			size_t i = 1;
			for (it = devices.begin();
				 it != devices.end();
				 ++it)
			{
				printf("\t%zu: %s%s\n", i, it->c_str(),
					   (i == 1 ? " (default)" : "") );
				++i;
			}
		}

		delete this;
		std::exit(EXIT_SUCCESS);
	}
}

void CLIScheduler::PrintErrorMsg(void) {
	switch (CLIScheduler::errno_) {
	  case OK:
	  case DAB_NOT_DETECTED:
	  case STATION_NOT_FOUND:
	    break;

	  case FILE_NOT_FOUND:
		ReadLine::printf("Specified input file wasn't found or couldn't be opened.\n");
		break;

	  case DEVICE_NOT_FOUND:
		if (this->user_input_->tuner_ > 0)
			ReadLine::printf("Specified tuner wasn't found.\n");
		else
			ReadLine::printf("No compatible tuners have been found.\n");
		break;

	  case DEVICE_DISCONNECTED:
	    ReadLine::printf("RTL tuner has been disconnected from USB.\n");
	    break;

	  case FILE_END:
		ReadLine::printf("In the supplied sample source file EOF has been reached.\n");
		break;

	  default:
		CLIScheduler::errno_ = ERROR_UNKNOWN;
		ReadLine::printf("Unknown error has occurred.\n");
	}
}

void CLIScheduler::CommandStation(const char *arg) {
	if (arg == NULL || arg[0] == '\0') { //get

		//it's ok not to check validity, as labels default to "Not Available"
		ReadLine::printf("  Current station: %s\n"
				"  Use 'station NEW CHANNEL NAME' to change current station.\n"
				"  Type 'list' to get list of stations "
				"available on current multiplex.\n",
				this->user_data_.station_label_.c_str());
	} else { //set
		//list is guaranteed not to be empty
		std::list<stationInfo> *stations = &this->user_data_.stations;
		uint8_t nr;

		if (1 == sscanf(arg, "%hhu", &nr) ) { //search by index
			for (std::list<stationInfo>::iterator it = stations->begin();
				 it != stations->end();
				 ++it)
			{
				if (nr == it->SubChannelId) {
					this->ParametersToSDR(it->SubChannelId);
					ReadLine::printf("  Changing station to: %s (#%hhu)\n",
									 it->station_name.c_str(), it->SubChannelId);
					return;
				}
			}
			ReadLine::printf("  Station #%hhu not found.\n", nr);
		} else { //search by name
			std::string name = UserData::DecodeEBULabel(arg);
			const stationInfo *station = NULL;
			bool found = false;

			for (std::list<stationInfo>::iterator it = stations->begin();
					it != stations->end() && !found;
					++it)
			{
				if (0 == strncmp(name.c_str(),
						it->station_name.c_str(),
						name.length()) )
				{
					if (found) {
						ReadLine::printf("  Station name \"%s\" is ambiguous.\n",
										 arg);
						return;
					}
					station = &(*it);
					found = true;
				}

			}
			if (found) {
				this->ParametersToSDR(station->SubChannelId);
				ReadLine::printf("  Changing station to: %s (#%hhu)\n",
								 station->station_name.c_str(),
								 station->SubChannelId);
			} else {
				ReadLine::printf("  Unknown station: \"%s\".\n"
								   "  Type 'list' to get list of stations "
								   "available on current multiplex.\n",
								 arg);
			}
		}
	}
}

void CLIScheduler::CommandList(void) {
	std::list<stationInfo> *stations = &this->user_data_.stations;
	//list is guaranteed not to be empty

	const char *after_line;

	for (std::list<stationInfo>::iterator it = stations->begin();
			it != stations->end();
			++it)
	{
		if (it->ServiceId == this->user_data_.station_id_)
			after_line = " (current station)";
		else
			after_line = "";
		ReadLine::printf("  %hhu.\t%s%s\n", it->SubChannelId,
				it->station_name.c_str(), after_line);
	}
}

bool CLIScheduler::SuppressStderr(void) {
	if (!this->user_input_->verbose_ && isatty(2)) { //2 is stderr
		//if stderr isn't explicitly requested to go somewhere
		return NULL == freopen("/dev/null", "w", stderr);
	} else {
		return true;
	}

}
