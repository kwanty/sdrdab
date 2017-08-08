#ifdef GOOGLE_UNIT_TEST

/*
 * THIS REQUIRES THE FOLLOWING COMMAND EXECUTED TO RUN
 * "echo 0 > /proc/sys/kernel/kptr_restrict"
 */


#include "gtest/gtest.h"
#include "scheduler.h"
#include <cstdlib>


class GprofTest  : public ::testing::Test{
    protected:

        Scheduler sch;

        GprofTest(){

        }

        virtual ~GprofTest(){
        }

        virtual void SetUp(){
        }

};


TEST_F(GprofTest, GprofTest) {
    // katowice - 209936000
    // krakow - 229072000

    printf("start\n");
    Scheduler sch;
    Scheduler::SchedulerConfig_t config;

    //const char * source = "./data/ut/Record3_katowice_iq.raw";
    const char * source = "./data/ut/antena-1_dab_229072kHz_fs2048kHz_gain42_1.raw";
    size_t freq = 229072000;

    config.data_source = Scheduler::DATA_FROM_FILE;
    config.input_filename = source;
    config.carrier_frequency = freq;

    sch.Start(config);

    printf("\nkoniec");
    return;
}


#endif

