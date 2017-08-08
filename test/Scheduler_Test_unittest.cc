/*
 * data_feeder_test.cc
 *
 *  Basic test of device
 *  Won't work if stick not connected
 *  Created on: Apr 21, 2015
 *      Author: hipek
 */

#ifdef GOOGLE_UNIT_TEST

#include "gtest/gtest.h"
#include "scheduler.h"

class SchedulerTest: public testing::Test {
    public:

        Scheduler sch;


        SchedulerTest(){

        }

        ~SchedulerTest(){

        }

    protected:

};

TEST_F(SchedulerTest, data_processing_test) {

    // katowice - 209936000
    // krakow - 229072000
    /*
       size_t frequency = 229072000;
       pthread_mutex_t lock_buffer;
       pthread_t mythread = 0;
       modeParameters mode_parameters;
       syncDetect sync_detect = {DAB_MODE_UNKNOWN, 0};
       syncRead sync_read;
       data_feeder_ctx_t data_write;
       demodReadWrite demod_read_write;
       decodReadWrite decod_read_write;
       syncFeedback sync_feedback;
       audioRead audio_read;
       std::list<stationInfo> station_info_list;
       stationInfo station_info = {1,"FIC",0,0,0,0,0};
       const char * source = "./data/ut/Record3_katowice_iq.raw";
    //const char * source = "./data/ut/antena-1_dab_229072kHz_fs2048kHz_gain42_1.raw";
    //const char * source = "0";


    entireData data = {lock_buffer, mythread, mode_parameters, sync_detect, sync_read, data_write,
    demod_read_write, decod_read_write, sync_feedback, audio_read, station_info_list,
    station_info, source, frequency};


    sch.Process(&data);
    */

}

#endif
