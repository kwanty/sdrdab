/*
 * synchronizer_SNR_unittest.cc
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 *
 * Miroslaw Szewczyk  szewczyk.mw@gmail.com
 */


//#ifdef GOOGLE_UNIT_TEST

#include <cmath>
#include "synchronizer.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class SynchronizerSNR_PR_Test  : public ::testing::Test{
protected:
    SynchronizerSNR_PR_Test(){

        mode_parameters.null_size = 2656;
        mode_parameters.frame_size = 196608;
        mode_parameters.guard_size = 504;

        mode_parameters.fft_size = 2048;
        mode_parameters.number_of_carriers = 1536;
        mode_parameters.number_cu_per_symbol = 48;
        mode_parameters.dab_mode = DAB_MODE_I;

        length_in = mode_parameters.fft_size + mode_parameters.guard_size;

        expectedResults = new float[3];
        in_infSNR = new float[length_in*2];
        in_40db = new float[length_in*2];
        in_20db = new float[length_in*2];
        in_2db = new float[length_in*2];

        synchro = new Synchronizer(&mode_parameters, 2048);
    }

    virtual ~SynchronizerSNR_PR_Test(){
        delete [] in_infSNR;
        delete [] in_40db;
        delete [] in_20db;
        delete [] in_2db;


        delete synchro;
    }

    virtual void SetUp(){
        loaded_in_infSNR = MatlabIO::ReadData( in_infSNR, "./data/ut/synchronizer_SNR_PR_in_infSNR.txt", length_in*2 );
        loaded_in_SNR_40db = MatlabIO::ReadData( in_40db, "./data/ut/synchronizer_SNR_PR_40db.txt", length_in*2 );
        loaded_in_SNR_20db = MatlabIO::ReadData( in_20db, "./data/ut/synchronizer_SNR_PR_20db.txt", length_in*2 );
        loaded_in_SNR_2db = MatlabIO::ReadData( in_2db, "./data/ut/synchronizer_SNR_PR_2db.txt", length_in*2 );
        loaded_expectedResults = MatlabIO::ReadData( expectedResults, "./data/ut/synchronizer_SNR_PR_expectedResults.txt", 4);
    }

    ModeParameters mode_parameters;

    size_t length_in;
    float *in_infSNR, *in_40db, *in_20db, *in_2db;
    float *expectedResults;
    bool loaded_in_infSNR,loaded_in_SNR_40db,loaded_in_SNR_20db, loaded_in_SNR_2db, loaded_expectedResults;
    transmissionMode mode;

    Synchronizer *synchro;
};

TEST_F(SynchronizerSNR_PR_Test, infSNR) {
    ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
    ASSERT_TRUE(loaded_in_infSNR)  << "TESTING CODE FAILED... could not load infSNR result";

    synchro->switchOnSNRfromPREFIX(true);
    synchro->calculateSNRfromPREFIX(in_infSNR);
    float snr = synchro->getSNRfromPREFIX();

    EXPECT_NEAR(expectedResults[0], snr, 0.3) << "wrong infSNR";
}

TEST_F(SynchronizerSNR_PR_Test, SNR40db) {
ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
ASSERT_TRUE(loaded_in_infSNR)  << "TESTING CODE FAILED... could not load 40dbSNR result";

synchro->switchOnSNRfromPREFIX(true);
synchro->calculateSNRfromPREFIX(in_40db);
float snr = synchro->getSNRfromPREFIX();

EXPECT_NEAR(expectedResults[1], snr, 0.3) << "wrong 40dbSNR";
}

TEST_F(SynchronizerSNR_PR_Test, SNR20db) {
ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
ASSERT_TRUE(loaded_in_infSNR)  << "TESTING CODE FAILED... could not load 20dbSNR result";

synchro->switchOnSNRfromPREFIX(true);
synchro->calculateSNRfromPREFIX(in_20db);
float snr = synchro->getSNRfromPREFIX();

EXPECT_NEAR(expectedResults[2], snr, 0.3) << "wrong 20dbSNR";
}

TEST_F(SynchronizerSNR_PR_Test, SNR2db) {
ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
ASSERT_TRUE(loaded_in_infSNR)  << "TESTING CODE FAILED... could not load 2dbSNR result";

synchro->switchOnSNRfromPREFIX(true);
synchro->calculateSNRfromPREFIX(in_2db);
float snr = synchro->getSNRfromPREFIX();

EXPECT_NEAR(expectedResults[3], snr, 0.3) << "wrong 2dbSNR";
}