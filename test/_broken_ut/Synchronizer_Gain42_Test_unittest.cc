/*
 * synchronizer_unittest.cc
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 */


#ifdef GOOGLE_UNIT_TEST

#include <cmath>
#include "synchronizer.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class SynchronizerGain42Test  : public ::testing::Test{
    protected:
        SynchronizerGain42Test(){

            mode_1_parameters.null_size = 2656;
            mode_1_parameters.frame_size = 196608;
            mode_1_parameters.guard_size = 504;

            mode_1_parameters.fft_size = 2048;
            mode_1_parameters.number_of_carriers = 1536;
            mode_1_parameters.number_cu_per_symbol = 48;
            mode_1_parameters.dab_mode = DAB_MODE_I;

            mode_2_parameters.null_size = 2656;
            mode_2_parameters.frame_size = 196608;
            mode_2_parameters.guard_size = 504;

            mode_2_parameters.guard_size = 126;
            mode_2_parameters.fft_size = 512;
            mode_2_parameters.number_of_carriers = 384;
            mode_2_parameters.number_cu_per_symbol = 12;
            mode_2_parameters.dab_mode = DAB_MODE_II;

            mode_3_parameters.null_size = 2656;
            mode_3_parameters.frame_size = 196608;
            mode_3_parameters.guard_size = 504;

            mode_3_parameters.guard_size = 63;
            mode_3_parameters.fft_size = 256;
            mode_3_parameters.number_of_carriers = 192;
            mode_3_parameters.number_cu_per_symbol = 6;
            mode_3_parameters.dab_mode = DAB_MODE_III;

            mode_4_parameters.null_size = 2656;
            mode_4_parameters.frame_size = 196608;
            mode_4_parameters.guard_size = 504;

            mode_4_parameters.guard_size = 252;
            mode_4_parameters.fft_size = 1024;
            mode_4_parameters.number_of_carriers = 768;
            mode_4_parameters.number_cu_per_symbol = 24;
            mode_4_parameters.dab_mode = DAB_MODE_IV;

            fc_drift = 0;
            fs_drift = 0;
            mode = DAB_MODE_UNKNOWN;
            null_pos = 0;

            dataLoaded1 = false;
            resultLoaded1 = false;

            dataLoaded2 = false;
            resultLoaded2 = false;

            dataLoaded3 = false;
            resultLoaded3 = false;

            length_in = 400000;
            length_out = 3;

            data1 = new float[length_in*2];
            expectedResults1 = new float[length_out];

            data2 = new float[length_in*2];
            expectedResults2 = new float[length_out];

            data3 = new float[length_in*2];
            expectedResults3 = new float[length_out];

            // detect mode

            mode_input_loaded = false;
            mode_output_loaded = false;

            mode_input_len = 524288;
            mode_input = new float[mode_input_len*2];
            mode_output = new float[1];


            syn_mode_unknown = new Synchronizer(mode_input_len);
            syn_mode_1 = new Synchronizer(&mode_1_parameters, 400000);
            syn_mode_2 = new Synchronizer(&mode_2_parameters, 400000);
            syn_mode_3 = new Synchronizer(&mode_3_parameters, 400000);
            syn_mode_4 = new Synchronizer(&mode_4_parameters, 400000);
        }

        virtual ~SynchronizerGain42Test(){
            delete [] data1;
            delete [] expectedResults1;

            delete [] data2;
            delete [] expectedResults2;

            delete [] data3;
            delete [] expectedResults3;

            delete [] mode_input;
            delete [] mode_output;

            delete syn_mode_unknown;
            delete syn_mode_1;
            delete syn_mode_2;
            delete syn_mode_3;
            delete syn_mode_4;
        }

        virtual void SetUp(){
            dataLoaded1 = MatlabIO::ReadData( data1, "./data/ut/synchronizer_null_input_1_gain42.txt", length_in*2 );
            resultLoaded1 = MatlabIO::ReadData( expectedResults1, "./data/ut/synchronizer_null_output_1_gain42.txt", length_out );

            dataLoaded2 = MatlabIO::ReadData( data2, "./data/ut/synchronizer_null_input_2_gain42.txt", length_in*2 );
            resultLoaded2 = MatlabIO::ReadData( expectedResults2, "./data/ut/synchronizer_null_output_2_gain42.txt", length_out );

            dataLoaded3 = MatlabIO::ReadData( data3, "./data/ut/synchronizer_null_input_3_gain42.txt", length_in*2 );
            resultLoaded3 = MatlabIO::ReadData( expectedResults3, "./data/ut/synchronizer_null_output_3_gain42.txt", length_out );

            mode_input_loaded = MatlabIO::ReadData( mode_input, "./data/ut/synchronizer_mode_input_gain42.txt", mode_input_len*2 );
            mode_output_loaded = MatlabIO::ReadData( mode_output, "./data/ut/synchronizer_mode_output_gain42.txt", 1 );
        }

        ModeParameters mode_1_parameters, mode_2_parameters, mode_3_parameters, mode_4_parameters;
        syncFeedback out_feedback;
        syncDetect out_detect;

        size_t length_in, length_out;
        float *data1, *data2, *data3;
        float *expectedResults1, *expectedResults2, *expectedResults3;
        bool dataLoaded1, dataLoaded2, dataLoaded3;
        bool resultLoaded1, resultLoaded2, resultLoaded3;

        size_t null_pos;
        float fs_drift;
        float fc_drift;

        bool mode_input_loaded, mode_output_loaded;
        float *mode_input, *mode_output;
        size_t mode_input_len;
        transmissionMode mode;


        Synchronizer *syn_mode_unknown;
        Synchronizer *syn_mode_1, *syn_mode_2, *syn_mode_3, *syn_mode_4;
};

// Mode detect

TEST_F(SynchronizerGain42Test, mode_detect) {
    ASSERT_TRUE(mode_input_loaded) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(mode_output_loaded)  << "TESTING CODE FAILED... could not load expected result";

    syn_mode_unknown->DetectMode(mode_input, mode_input_len, &out_detect);

    EXPECT_EQ(mode_output[0], out_detect.mode) << "mode "<< out_detect.mode << " vs " << mode_output[0];
}

// NULL detect

TEST_F(SynchronizerGain42Test, null_detect_frame_1) {
    ASSERT_TRUE(dataLoaded1) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(resultLoaded1)  << "TESTING CODE FAILED... could not load expected result";

    syn_mode_1->DetectAndDecodeNULL(data1, length_in);

    null_pos = syn_mode_1->null_position_;

    EXPECT_EQ(expectedResults1[0], null_pos) << "null position "<< null_pos << " vs " << expectedResults1[0];
}

TEST_F(SynchronizerGain42Test, null_detect_frame_2) {
    ASSERT_TRUE(dataLoaded2) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(resultLoaded2)  << "TESTING CODE FAILED... could not load expected result";

    syn_mode_1->DetectAndDecodeNULL(data2, length_in);

    null_pos = syn_mode_1->null_position_;

    EXPECT_EQ(expectedResults2[0], null_pos) << "null position "<< null_pos << " vs " << expectedResults2[0];
}

TEST_F(SynchronizerGain42Test, null_detect_frame_3) {
    ASSERT_TRUE(dataLoaded3) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(resultLoaded3)  << "TESTING CODE FAILED... could not load expected result";

    syn_mode_1->DetectAndDecodeNULL(data3, length_in);

    null_pos = syn_mode_1->null_position_;

    EXPECT_EQ(expectedResults3[0], null_pos) << "null position "<< null_pos << " vs " << expectedResults3[0];
}


// Process

TEST_F(SynchronizerGain42Test, process_frame_1) {
    ASSERT_TRUE(dataLoaded1) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(resultLoaded1)  << "TESTING CODE FAILED... could not load expected result";

    syn_mode_1->Process(data1, length_in, &out_feedback);

    null_pos = out_feedback.null_position;
    fc_drift = syn_mode_1->fc_drift_;

    EXPECT_EQ(expectedResults1[0], null_pos) << "null position "<< null_pos << " vs " << expectedResults1[0];
    EXPECT_EQ(expectedResults1[1] + expectedResults1[2], fc_drift) << "fc drift "<< fc_drift << " vs " << expectedResults1[1] + expectedResults1[2];

}

TEST_F(SynchronizerGain42Test, process_frame_2) {
    ASSERT_TRUE(dataLoaded2) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(resultLoaded2)  << "TESTING CODE FAILED... could not load expected result";

    syn_mode_1->Process(data2, length_in, &out_feedback);

    null_pos = out_feedback.null_position;
    fc_drift = syn_mode_1->fc_drift_;

    EXPECT_EQ(expectedResults2[0], null_pos) << "null position "<< null_pos << " vs " << expectedResults2[0];
    EXPECT_EQ(expectedResults2[1] + expectedResults2[2], fc_drift) << "fc drift "<< fc_drift << " vs " << expectedResults2[1] + expectedResults2[2];

}

TEST_F(SynchronizerGain42Test, process_frame_3) {
    ASSERT_TRUE(dataLoaded3) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(resultLoaded3)  << "TESTING CODE FAILED... could not load expected result";

    syn_mode_1->Process(data3, length_in, &out_feedback);

    null_pos = out_feedback.null_position;
    fc_drift = syn_mode_1->fc_drift_;

    EXPECT_EQ(expectedResults3[0], null_pos) << "null position "<< null_pos << " vs " << expectedResults3[0];
    EXPECT_EQ(expectedResults3[1] + expectedResults3[2], fc_drift) << "fc drift "<< fc_drift << " vs " << expectedResults3[1] + expectedResults3[2];

}
#endif
