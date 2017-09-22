/**
 * @class UserInput
 * @brief UserInput implementation, global argp configuration
 *
 * @author Krzysztof Szczęsny, kaszczesny@gmail.com
 * @author Miroslaw Szewczyk, mirsze@student.agh.edu.pl
 * @date Created on: 1 May 2015 - version 1.0
 * @date 7 July 2017 - version 3.0
 * @copyright Copyright (c) 2015 Krzysztof Szczęsny
 * @copyright Copyright (c) 2017 Miroslaw Szewczyk added protocol for resampling
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

#include "user_input.h"
/// @cond
#include <cstring>
#include <cstdlib> //std::exit
/// @endcond
#include <cctype> //tolower
#include <string>

/**
 * @name Global argp configuration
 */
///@{
const char *argp_program_version = "sdrdab-cli 1.0";
const char *argp_program_bug_address =
"<https://sdr.kt.agh.edu.pl/sdrdab#contact>";
error_t argp_err_exit_status = UserInput::ARGP_EXIT_ERR_CODE;
///@}


UserInput::UserInput()
    : freq_(0U),
    sampling_rate_(0U),
    channel_nr(0U),
    list_(false),
    tuner_(NO_TUNER_SELECTED),
    file_(NULL),
    silent_(false),
    rds_(true),
    info_(false),
    verbose_(false),
    graphic_(NULL),
    output_(NULL),
    resamplingFreq_(NULL),
    resample_quality_(Resampler2::LINEAR),          // default resampling type if not specified
    decodingAlg_(DataDecoder::ALG_UNSPEC),          // default decoding algorithm if not specified
    bugfix_applied_(false) {

    }

UserInput::~UserInput() {
    //all strings are allocated by argp
}

void UserInput::Process(int argc, char **argv) {
    if (this->RunArgp(argc, argv) != 0) {
        std::exit(argp_err_exit_status);
    }
}

int UserInput::RunArgp(int argc, char **argv) {
    argp_option options[] = {
        { 0,                0,   0,         0, "Core radio options:", 1 },
        { "frequency",     'f', "FREQ",     0, "Carrier frequency in kHz. Default value: 229072 (DAB+ in Krakow)" },
        { "sampling-rate", 420, "FS",       OPTION_HIDDEN, "Sampling rate in kHz. Default value: 2048"},
        { "channel",       'c', "CHANN",    0, "Optionally start decoding specified channel instead of the first one. This requires a number, not name" },
        { 0,                0,   0,         0, "Input options:", 2 },
        { "list-devices",  'l',  0,         OPTION_NO_USAGE, "Lists all available supported tuners and exits" },
        { "open-tuner",    't', "TUNER_NO", OPTION_ARG_OPTIONAL | OPTION_NO_USAGE, "Use n-th available tuner as input source (by default first tuner is used)" },
        { "open-file",     421, "FILENAME", OPTION_NO_USAGE, "Use FILENAME as input source" },
        { "file-type",     424, "FILETYPE", OPTION_NO_USAGE, "Use FILETYPE={int8, uint8, int16, uint16, float, double, raw} to define how read samples from file, raw==uint8" },
        { 0,                0,   0,         0, "Output options:", 3 },
        { "silent",        's',  0,         0, "Don't play on speakers (applicable in conjunction with -o switch)" },
        { "no-rds",        'r',  0,         0, "Don't display current channel RDS" },
        { "detailed-info", 'i',  0,         0, "Print verbosely channel information every time ENTER is pressed" },
        { "graphic",       'g', "GRAPHIC",  0, "Save SlideShow images in GRAPHIC directory" },
        { "output",        'o', "OUTPUT",   OPTION_NO_USAGE, "Save audio to OUTPUT file" },
        { "katowice",      422,  0,         OPTION_HIDDEN, "Setup for Record3_katowice_iq.raw"},
        { "antena",        423,  0,         OPTION_HIDDEN, "Setup for antena-1_dab_229072kHz_fs2048kHz_gain42_1.raw"},
        {0,                 0,   0,         0, "Algorithm options:", 4},
        { "resampling",  500,  "TYPE",  0,  "Use TYPE={nearest, linear, pchip, spline}. Only when read samples from file, pchip and spline are not yet supported, default is linear"},
        { "convolutional",  501,  "CONVOLUTIONAL",  0,  "Decoding algorithms: CONVOLUTIONAL={viterbi, spiral, hard}. Default is viterbi, spiral and hard are not yet supported"},
        { 0,                0,   0,         0, "Informative options:", -1},
        { "verbose",       'v',  0,         0, "Verbose" },
        { "open-file",     421, "FILENAME", OPTION_NO_USAGE, "Use FILENAME as input source" },
        { 0 }
    };

    argp argp_config = {
        options,
        UserInput::Parser,
        "\n-l\tlist available devices"
            "\n[-t[TUNER_NO=0]]\tuse n-th tuner"
            "\n--open-file=FILENAME\tread samples from FILENAME"
            "\n--resampling_mode=nearest/linear/pchip/spline\tchoose specified resampling mode"
            "\n--convolutional=viterbi/spiral/hard\tchoose algorithm to decode"
            "\n [-s] -o OUTPUT\tsave audio to OUPUT",
        "Exemplary application of sdrdab\v"
            "\nThis application shows how sdrdab can be used. It allows "
            "user to decode DAB/DAB+ signal either from RTL2832U tuner or from "
            "raw samples file (available on https://sdr.kt.agh.edu.pl/sdrdab/downloads/data). "
            "Audio stream can be played immediately on speakers and/or saved to "
            "file. Program can display basic channel information in RDS-like "
            "form or print very detailed information.\n\n"
            "After being started, this program goes into interactive command-line "
            "mode. Type 'help' and press Enter to get list of supported commands.\n\n"
            "To learn more about DAB/DAB+ capabilities and about sdrdab, "
            "visit our website: <https://sdr.kt.agh.edu.pl/sdrdab>.\n\n"
            "This program is licensed under GPLv3 license; for more information "
            "see <http://www.gnu.org/licenses/gpl-3.0.en.html>."
    };

    return argp_parse(&argp_config, argc, argv, 0, 0, static_cast<void *>(this));
}

void UserInput::Postprocessing(argp_state *state) {
    if (this->list_) {
        return;
    }

    if (this->silent_ && this->output_ == NULL) {
        argp_error(state,
                "You cannot use -s flag without specifying the -o OUTPUT option.");
    }

    if (this->file_type_ && this->file_ == NULL) {
        argp_error(state,
                   "You cannot use --file-type flag without specifying the --open-file flag.");
    }



    if (this->verbose_) {
        fprintf(stderr, "Input notices:\n");
    }

    if (this->FromTuner()) {
        if (this->tuner_ == NO_TUNER_SELECTED) {
            if (this->verbose_)
                fprintf(stderr, "\tAssuming first RTL tuner as source.\n");
            this->tuner_ = 0;
        }
        if (this->tuner_ == 0 && this->verbose_) {
            fprintf(stderr, "\tFirst RTL tuner selected.\n");
        }
        if (this->tuner_ > 0 && this->verbose_) {
            fprintf(stderr, "\tRTL tuner #%d selected.\n", this->tuner_);
        }
        if (this->file_ != NULL && this->verbose_) {
            fprintf(stderr,
                    "\tBoth tuner and file were selected - ignoring file.\n");
        }
    } else {
        if (this->verbose_) {
            fprintf(stderr, "\tReading samples from file: %s\n", this->file_);
        }

        if (!this->file_type_ && this->file_) {
            file_type_ = type_uint8;
            if (this->verbose_) {
                fprintf(stderr, "\tDefault file type is uint8.\n");
            }
        }
    }

    if (this->verbose_) {
        if (this->silent_ && this->output_ != NULL)
            fprintf(stderr, "\tSaving audio to file: %s\n", this->output_);

        if (!this->silent_ && this->output_ == NULL)
            fprintf(stderr, "\tPlaying audio on speakers.\n");

        if (!this->silent_ && this->output_ != NULL)
            fprintf(stderr,
                    "\tPlaying audio on speakers and saving it to file: %s\n",
                    this->output_);
    }

    //freq processing
    if (this->freq_ == 0U) {
        this->freq_ = UserInput::DEFAULT_FREQ_kHz * 1000U;
        if (this->verbose_)
            fprintf(stderr, "\tAssuming default frequency (%"SCNu32" kHz).\n",
                    UserInput::DEFAULT_FREQ_kHz);
    } else {
        if (this->verbose_)
            fprintf(stderr, "\tFrequency selected: %"SCNu32" kHz.\n",
                    this->freq_ / 1000U);
    }

    //sampling_rate processing
    if (this->sampling_rate_ == 0U) {
        this->sampling_rate_ = UserInput::DEFAULT_FS_kHz * 1000U;
        if (this->verbose_)
            fprintf(stderr, "\tAssuming default sampling rate (%"SCNu32" kHz).\n",
                    UserInput::DEFAULT_FS_kHz);
    } else {
        if (this->verbose_)
            fprintf(stderr, "\tSampling rate selected: %"SCNu32" kHz.\n",
                    this->sampling_rate_ / 1000U);
    }

    //all other class members verbose notices
    if (this->verbose_) {
        if (this->channel_nr != 0)
            fprintf(stderr, "\tInitial channel selected: \"%hhu\"\n",
                    this->channel_nr);
        if (!this->rds_)
            fprintf(stderr, "\tRDS display turned off.\n");
        if (this->info_)
            fprintf(stderr, "\tDetailed channel information turned on.\n");
        if (this->graphic_ != NULL)
            fprintf(stderr, "\tSaving images to: %s\n", this->graphic_);
        if(this->resamplingFreq_ != NULL) {
            fprintf(stderr, "\tResampling frequency is: %s\n", this->resamplingFreq_);
        }
        if(this->decodingAlg_ != DataDecoder::ALG_UNSPEC) {
        	string alg_name;
        	switch(this->decodingAlg_){
				case DataDecoder::ALG_VITERBI_TZ:
					alg_name = "ALG_VITERBI_TZ";
					break;
				default:
					alg_name = "ALG_UNSPEC";
        	}
            fprintf(stderr, "\tusing %s algorithm\n", alg_name.c_str());
        }
    }

    //set default convolutional decoder
    if (this->decodingAlg_ == DataDecoder::ALG_UNSPEC) {
        this->decodingAlg_ = DataDecoder::ALG_VITERBI_TZ;
    }
}

int UserInput::Parser(int key, char *arg, argp_state *state) {
    if (state == NULL) {
        return EINVAL;//user_input->resamplingFreq_ = arg;
    }

    UserInput *user_input = static_cast<UserInput *>(state->input);

    switch (key) {
        case 'f':
            //check parsing failure
            if (sscanf(arg, "%"SCNu32, &user_input->freq_) != 1) {
                argp_error(state, "Invalid frequency specified.");
            }
            if (user_input->freq_ < MIN_DAB_FREQ_kHz ||
                    user_input->freq_ > MAX_DAB_FREQ_kHz )
            {
                argp_error(state,
                        "Specified frequency is outside possible DAB band (174-230 MHz).");
            }
            user_input->freq_ *= 1000U; //kHz to Hz
            break;

        case 'c':
            if (sscanf(arg, "%hhu", &user_input->channel_nr) != 1) {
                argp_error(state,
                        "Invalid initial channel specified (this option expects a number).");
            }
            break;

        case 'l':
            user_input->list_ = true;
            break;

        case 't':
            user_input->tuner_ = 0; //indicate that it was 'touched'
            if (arg == NULL) {
                user_input->ApplyBugfix();
            }
            else {
                /* consume leading '=' produced by i.e. '-t=0', as equality
                 * sign is by default interpreted as part of argument in this
                 * situation */
                const char *arg_shifted = (arg[0] == '=' ? arg+1 : arg);

                //check parsing failure
                if (sscanf(arg_shifted, "%d", &user_input->tuner_) != 1 ||
                        user_input->tuner_ < 0)
                {
                    argp_error(state, "Invalid tuner number.");
                }
            }
            break;

        case 420: //--sampling-rate
            //check parsing failure
            if (sscanf(arg, "%"SCNu32, &user_input->sampling_rate_) != 1) {
                argp_error(state, "Invalid sampling rate specified.");
            }
            if (false) { ///@todo valid sampling rate range
                argp_error(state, "Specified sampling rate is outside possible value range.");
            }
            user_input->sampling_rate_ *= 1000U; //kHz to Hz
            break;


        case 421: //--open-file
            if (arg[0] == '\0')
                argp_error(state,
                        "--open-file flag was given, but empty path was specified.");
            user_input->file_ = arg;
            break;

        case 422: // --katowice
            user_input->freq_ = 209936;
            user_input->file_ = "./data/Record3_katowice_iq.raw";
            user_input->channel_nr = 6; //jazz
            user_input->output_ = "out-kat.ogg";
            break;

        case 423: //--antena
            user_input->freq_ = 229072;
            user_input->file_ = "./data/antena-1_dab_229072kHz_fs2048kHz_gain42_1.raw";
            user_input->output_ = "out-ant.ogg";
            break;

        case 424: //--file-type
            if (arg[0] == '\0') {
                argp_error(state,
                           "--file-type flag was given, but empty type was specified.");
            } else if (strcmp(arg, "int8") == 0) {
                user_input->file_type_ = type_int8;
            } else if (strcmp(arg, "uint8") == 0) {
                user_input->file_type_ = type_uint8;
            } else if (strcmp(arg, "int16") == 0) {
                user_input->file_type_ = type_int16;
            } else if (strcmp(arg, "uint16") == 0) {
                user_input->file_type_ = type_uint16;
            } else if (strcmp(arg, "float") == 0) {
                user_input->file_type_ = type_float;
            } else if (strcmp(arg, "double") == 0) {
                user_input->file_type_ = type_double;
            } else if (strcmp(arg, "raw") == 0) {
                user_input->file_type_ = type_raw;
            } else {
                argp_error(state,
                           "Bad sample type given [raw, uint8, int8, uint16, int16, float, double]");
            }
            break;

        case 500:
            if (strcmp(arg, "nearest") == 0) {
                user_input->resample_quality_ = Resampler2::NN;
            } else if (strcmp(arg, "linear") == 0) {
                user_input->resample_quality_ = Resampler2::LINEAR;
            } else if (strcmp(arg, "pchip") == 0) {
                // user_input->resample_quality_ = Resampler2::PCHIP;
                argp_error(state, "Resampling pchip is not supported.");
            } else if (strcmp(arg, "spline") == 0) {
                // user_input->resample_quality_ = Resampler2::SPLINE;
                argp_error(state, "Resampling spline is not supported.");
            }else {
                argp_error(state, "Resampling quality is not specified. Use: nearest, linear (default), pchip (not supported), spline (not supported)");
            }
            break;

        case 501:
            if(strcmp(arg, "viterbi") == 0) {
                user_input->decodingAlg_ = DataDecoder::ALG_VITERBI_TZ;
            } else if (strcmp(arg, "spiral") == 0){
                argp_error(state, "Spiral algorithm not supported.");
            } else if (strcmp(arg, "hard") == 0){
                argp_error(state, "Hard algorithm not supported.");
            } else {
                argp_error(state, "Algorithm not specified.");
            }
            break;

        case 's':
            user_input->silent_ = true;
            break;

        case 'r':
            user_input->rds_ = false;
            break;

        case 'g':
            user_input->graphic_ = arg;
            break;

        case 'o':
            user_input->output_ = arg;
            break;

        case 'i':
            user_input->info_ = true;
            break;

        case 'v':
            user_input->verbose_ = true;
            break;

        case ARGP_KEY_ARG:
            //ARGP_KEY_ARG is guaranteed to be parsed after option keys
            if (user_input->list_) {
                break;
            }

            if (user_input->bugfix_applied_) {
                fprintf(stderr,
                        "WARNING: assuming that \"%s\" refers to tuner number. "
                        "To avoid this message, always use \"-tTUNER_NO\" or "
                        "\"--open-tuner=TUNER_NO\".\n",
                        arg);
                if (sscanf(arg, "%d", &user_input->tuner_) != 1 ||
                        user_input->tuner_ < 0)
                {
                    argp_error(state, "Invalid tuner number.");
                }
            } else {
                argp_error(state, "All arguments should be given via options.");
            }
            break;

        case ARGP_KEY_INIT:
        case ARGP_KEY_END:
        case ARGP_KEY_ARGS:
        case ARGP_KEY_NO_ARGS:
        case ARGP_KEY_SUCCESS:
            break;

        case ARGP_KEY_FINI:
            user_input->Postprocessing(state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

bool UserInput::FromTuner(void) {
    if (this->tuner_ != NO_TUNER_SELECTED)
        return true;

    if (this->file_ != NULL)
        return false;
    else
        return true;
}

