/*
 * user_conf.cc
 *
 *  Created on: 16 maj 2016
 *
 */

#include "user_conf.h"


UserConf::UserConf()
    : freq_(0U),
      sampling_rate_(0U),
      channel_nr(0U),
      list_(false),
      from_tuner_(true),
      tuner_(NO_TUNER_SELECTED),
      file_(NULL),
      silent_(false),
      rds_(true),
      info_(false),
      verbose_(false),
      graphic_(NULL),
      output_(NULL){
}

UserConf::~UserConf(){
}

void UserConf::CompleteEmptyValues(){
    if (this->list_) {
        return;
    }
    if (this->verbose_) {
        fprintf(stderr, " notices:\n");
    }
    if (this->from_tuner_) {
        if (this->tuner_ == NO_TUNER_SELECTED) {
            if (this->verbose_)
                fprintf(stderr, "\tAssuming first RTL tuner as source.\n");
            //this->tuner_ = 0;
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
        this->freq_ = UserConf::DEFAULT_FREQ_kHz * 1000U;
        if (this->verbose_)
            fprintf(stderr, "\tAssuming default frequency (%"SCNu32" kHz).\n",
                    UserConf::DEFAULT_FREQ_kHz);
    } else {
        if (this->verbose_)
            fprintf(stderr, "\tFrequency selected: %"SCNu32" kHz.\n",
                    this->freq_ / 1000U);
    }

    //sampling_rate processing
    if (this->sampling_rate_ == 0U) {
        this->sampling_rate_ = UserConf::DEFAULT_FS_kHz * 1000U;
        if (this->verbose_)
            fprintf(stderr, "\tAssuming default sampling rate (%"SCNu32" kHz).\n",
                    UserConf::DEFAULT_FS_kHz);
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
    }
}

