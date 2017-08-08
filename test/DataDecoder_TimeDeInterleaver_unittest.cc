/*
   ============================================================================
Author      : Biela Ernest
E-mail      : ernest.biela@gmail.com
Date        : 30.06.2015
============================================================================
*/
#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include <sys/time.h>

using namespace std;

/**********************time check functions******************************/
static double tic_time;

void tic() {
    struct timeval tp;
    gettimeofday(&tp,0);
    tic_time= tp.tv_usec;
}

double toc() {
    struct timeval tp;
    gettimeofday(&tp,0);
    return tp.tv_usec-tic_time;
}
/**********************end time check functions******************************/

class TimeDeInterleaverTest  : public ::testing::Test{
    protected:
        TimeDeInterleaverTest(){
            param_mode_1.guard_size = 504;
            param_mode_1.fft_size = 2048;
            param_mode_1.symbol_size = 2552;
            param_mode_1.number_of_symbols = 76;
            param_mode_1.null_size = 2656;
            param_mode_1.frame_size = 196608;
            param_mode_1.number_of_carriers = 1536;
            param_mode_1.number_of_symbols_per_fic = 3;
            param_mode_1.number_of_fib = 12;
            param_mode_1.number_of_cif = 4;
            param_mode_1.number_of_deqpsk_unit_for_read = 5;
            param_mode_1.number_of_fib_per_cif = 3;
            param_mode_1.number_samp_after_timedep = 3096;
            param_mode_1.number_samp_after_vit = 768;
            param_mode_1.sync_read_size = 199264;
            param_mode_1.fic_size = 9216;
            param_mode_1.number_cu_per_symbol = 48;
            param_mode_1.number_symbols_per_cif = 18;
            param_mode_1.dab_mode = DAB_MODE_I;

            param_mode_2_3.guard_size = 126;
            param_mode_2_3.fft_size = 512;
            param_mode_2_3.symbol_size = 638;
            param_mode_2_3.number_of_symbols = 76;
            param_mode_2_3.null_size = 664;
            param_mode_2_3.frame_size = 49152;
            param_mode_2_3.number_of_carriers = 384;
            param_mode_2_3.number_of_symbols_per_fic = 3;
            param_mode_2_3.number_of_fib = 3;
            param_mode_2_3.number_of_cif = 1;
            param_mode_2_3.number_of_deqpsk_unit_for_read = 17;
            param_mode_2_3.number_of_fib_per_cif = 3;
            param_mode_2_3.number_samp_after_timedep = 3096;
            param_mode_2_3.number_samp_after_vit = 768;
            param_mode_2_3.sync_read_size = 49816;
            param_mode_2_3.fic_size = 2304;
            param_mode_2_3.number_cu_per_symbol = 12;
            param_mode_2_3.number_symbols_per_cif = 72;
            param_mode_2_3.dab_mode = DAB_MODE_II;

            inputlength_mode_1 = 241920;
            outputlength_mode_1 = 26880;

            inputlength_mode_2_3 = 278784;
            outputlength_mode_2_3 = 8448;

            expectedinputdata_mode_1 = new float[inputlength_mode_1];
            expectedoutputdata_mode_1 = new float[outputlength_mode_1];

            expectedinputdata_mode_2_3 = new float[inputlength_mode_2_3];
            expectedoutputdata_mode_2_3 = new float[outputlength_mode_2_3];

            inputdata_mode_1 = expectedinputdata_mode_1;
            outputdata_mode_1 = expectedinputdata_mode_1;

            inputdata_mode_2_3 = expectedinputdata_mode_2_3;
            outputdata_mode_2_3 = expectedinputdata_mode_2_3;

            inputdataLoaded_mode_1 = false;
            outputdataLoaded_mode_1 = false;

            inputdataLoaded_mode_2_3 = false;
            outputdataLoaded_mode_2_3 = false;

            info.audio_kbps = 112;
            info.sub_ch_size = 72;
            info.protection_level = 2;
            info.IsLong = true;
            info.ProtectionLevelTypeB = false;
            info.sub_ch_start_addr = 0;

            datadecoder_mode_1 = new DataDecoder(&info, &param_mode_1, DataDecoder::ALG_VITERBI_TZ);
            datadecoder_mode_2_3 = new DataDecoder(&info, &param_mode_2_3, DataDecoder::ALG_VITERBI_TZ);
        }

        virtual ~TimeDeInterleaverTest(){
            delete datadecoder_mode_1;
            delete datadecoder_mode_2_3;
            delete [] expectedinputdata_mode_1;
            delete [] expectedoutputdata_mode_1;
        }

        virtual void SetUp(){
            inputdataLoaded_mode_1 = MatlabIO::ReadData(expectedinputdata_mode_1, "./data/ut/timedeinterleaver_data_in_mode_1.txt", inputlength_mode_1);
            outputdataLoaded_mode_1 = MatlabIO::ReadData(expectedoutputdata_mode_1, "./data/ut/timedeinterleaver_data_out_mode_1.txt", outputlength_mode_1);

            inputdataLoaded_mode_2_3 = MatlabIO::ReadData(expectedinputdata_mode_2_3, "./data/ut/timedeinterleaver_data_in_mode_2&3.txt", inputlength_mode_2_3);
            outputdataLoaded_mode_2_3 = MatlabIO::ReadData(expectedoutputdata_mode_2_3, "./data/ut/timedeinterleaver_data_out_mode_2&3.txt", outputlength_mode_2_3);
        }

        size_t inputlength_mode_1;
        size_t outputlength_mode_1;

        size_t inputlength_mode_2_3;
        size_t outputlength_mode_2_3;

        bool inputdataLoaded_mode_1;
        bool outputdataLoaded_mode_1;

        bool inputdataLoaded_mode_2_3;
        bool outputdataLoaded_mode_2_3;

        float* expectedinputdata_mode_1;
        float* expectedoutputdata_mode_1;

        float* expectedinputdata_mode_2_3;
        float* expectedoutputdata_mode_2_3;

        float* inputdata_mode_1;
        float* outputdata_mode_1;

        float* inputdata_mode_2_3;
        float* outputdata_mode_2_3;

        ModeParameters param_mode_1;
        ModeParameters param_mode_2_3;
        stationInfo info;
        DataDecoder * datadecoder_mode_1;
        DataDecoder * datadecoder_mode_2_3;
};


TEST_F(TimeDeInterleaverTest, MODE_1) {
    ASSERT_TRUE(inputdataLoaded_mode_1) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_mode_1) << "TESTING CODE FAILED... could not load output data";
    for (size_t i = 0; i < 5; i++)
    {
        //tic();
        datadecoder_mode_1->TimeDeInterleaver(inputdata_mode_1);
        inputdata_mode_1 += outputlength_mode_1;
        //toc();
    }
    outputdata_mode_1 += 4 * outputlength_mode_1;
    for (size_t i = 0; i < outputlength_mode_1; i++) {
        EXPECT_EQ(outputdata_mode_1[i], expectedoutputdata_mode_1[i]) << "vectors differ on " << i;
    }
}

TEST_F(TimeDeInterleaverTest, MODE_2_3) {
    ASSERT_TRUE(inputdataLoaded_mode_1) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_mode_2_3) << "TESTING CODE FAILED... could not load output data";

    for (size_t i = 0; i < 17; i++)
    {
        datadecoder_mode_2_3->TimeDeInterleaver(inputdata_mode_2_3);
        inputdata_mode_2_3 += outputlength_mode_2_3;
    }
    outputdata_mode_2_3 += 16 * outputlength_mode_2_3;
    for (size_t i = 0; i < outputlength_mode_2_3; i++) {
        EXPECT_EQ(outputdata_mode_2_3[i], expectedoutputdata_mode_2_3[i]) << "vectors differ on " << i;
    }
}


#endif
