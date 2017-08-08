/*
 * synchronizer_SNR_unittest.cc
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 *
 * Jaroslaw Bulat kwant@agh.edu.pl 20.06.2016
 */


//#ifdef GOOGLE_UNIT_TEST

#include <cmath>
#include "synchronizer.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class SynchronizerSNRTest  : public ::testing::Test{
    protected:
    SynchronizerSNRTest(){

            mode_parameters.null_size = 2656;
            mode_parameters.frame_size = 196608;
            mode_parameters.guard_size = 504;

            mode_parameters.fft_size = 2048;
            mode_parameters.number_of_carriers = 1536;
            mode_parameters.number_cu_per_symbol = 48;
            mode_parameters.dab_mode = DAB_MODE_I;

            length_in = mode_parameters.fft_size;

            expectedResults = new float[5];
            in_zeroSNR = new float[length_in*2];
            in_lowSNR = new float[length_in*2];
            in_highSNR = new float[length_in*2];
            in_infSNR = new float[length_in*2];
            in_minusInfSNR = new float[length_in*2];
            synchro = new Synchronizer(&mode_parameters, 2048);
        }

        virtual ~SynchronizerSNRTest(){
            delete [] in_zeroSNR;
            delete [] in_lowSNR;
            delete [] in_highSNR;
            delete [] in_infSNR;
            delete [] in_minusInfSNR;

            delete synchro;
        }

        virtual void SetUp(){
            loaded_in_zeroSNR = MatlabIO::ReadData( in_zeroSNR, "./data/ut/synchronizer_SNR_in_zeroSNR.txt", length_in*2 );
            loaded_in_lowSNR = MatlabIO::ReadData( in_lowSNR, "./data/ut/synchronizer_SNR_in_lowSNR.txt", length_in*2 );
            loaded_in_highSNR = MatlabIO::ReadData( in_highSNR, "./data/ut/synchronizer_SNR_in_highSNR.txt", length_in*2 );
            loaded_in_infSNR = MatlabIO::ReadData( in_infSNR, "./data/ut/synchronizer_SNR_in_infSNR.txt", length_in*2 );
            loaded_in_minusInfSNR = MatlabIO::ReadData( in_minusInfSNR, "./data/ut/synchronizer_SNR_in_minusInfSNR.txt", length_in*2 );
            loaded_expectedResults = MatlabIO::ReadData( expectedResults, "./data/ut/synchronizer_SNR_expectedResults.txt", 5 );
        }

        ModeParameters mode_parameters;

        size_t length_in;
        float *in_zeroSNR, *in_lowSNR, *in_highSNR, *in_infSNR, *in_minusInfSNR;
        float *expectedResults;
        bool loaded_in_zeroSNR, loaded_in_lowSNR, loaded_in_highSNR, loaded_in_infSNR, loaded_in_minusInfSNR, loaded_expectedResults;
        transmissionMode mode;

        Synchronizer *synchro;
};

// switchONSNR(false)

TEST_F(SynchronizerSNRTest, zeroSNR) {
    ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
    ASSERT_TRUE(loaded_in_zeroSNR)  << "TESTING CODE FAILED... could not load zeroSNR result";

    synchro->switchOnSNRfromSPECTRUM(true);
    synchro->calculateSNRfromSPECTRUM(in_zeroSNR);
    float snr = synchro->getSNRfromSPECTRUM();

    EXPECT_NEAR(expectedResults[0], snr, 0.2) << "wrong zeroSNR";
}


TEST_F(SynchronizerSNRTest, lowSNR) {
    ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
    ASSERT_TRUE(loaded_in_lowSNR)  << "TESTING CODE FAILED... could not load lowSNR result";

    synchro->switchOnSNRfromSPECTRUM(true);
    synchro->calculateSNRfromSPECTRUM(in_lowSNR);
    float snr = synchro->getSNRfromSPECTRUM();

    EXPECT_NEAR(expectedResults[1], snr, expectedResults[1]*0.1) << "wrong lowSNR";
}


TEST_F(SynchronizerSNRTest, highSNR) {
    ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
    ASSERT_TRUE(loaded_in_highSNR)  << "TESTING CODE FAILED... could not load highSNR result";

    synchro->switchOnSNRfromSPECTRUM(true);
    synchro->calculateSNRfromSPECTRUM(in_highSNR);
    float snr = synchro->getSNRfromSPECTRUM();

    EXPECT_NEAR(expectedResults[2], snr, expectedResults[2]*0.1) << "wrong lowSNR";
}


TEST_F(SynchronizerSNRTest, infSNR) {
    ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
    ASSERT_TRUE(loaded_in_infSNR)  << "TESTING CODE FAILED... could not load infSNR result";

    synchro->switchOnSNRfromSPECTRUM(true);
    synchro->calculateSNRfromSPECTRUM(in_infSNR);
    float snr = synchro->getSNRfromSPECTRUM();

    EXPECT_EQ(expectedResults[3], snr) << "wrong infSNR";
}


TEST_F(SynchronizerSNRTest, minusInfSNR) {
    ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
    ASSERT_TRUE(loaded_in_minusInfSNR)  << "TESTING CODE FAILED... could not load minusInfSNR result";

    synchro->switchOnSNRfromSPECTRUM(false);
    synchro->calculateSNRfromSPECTRUM(in_minusInfSNR);
    float snr = synchro->getSNRfromSPECTRUM();

    EXPECT_EQ(expectedResults[4], snr) << "wrong minusInfSNR";
}

TEST_F(SynchronizerSNRTest, snrSwitchOFF) {
    ASSERT_TRUE(loaded_expectedResults) << "TESTING CODE FAILED... could not load expected results";
    ASSERT_TRUE(loaded_in_minusInfSNR)  << "TESTING CODE FAILED... could not load minusInfSNR result";

    synchro->switchOnSNRfromSPECTRUM(false);
    synchro->calculateSNRfromSPECTRUM(in_minusInfSNR);
    float snr = synchro->getSNRfromSPECTRUM();

    EXPECT_EQ(expectedResults[4], snr) << "wrong minusInfSNR";
}

TEST_F(SynchronizerSNRTest, snrNotStarted) {
    synchro->switchOnSNRfromSPECTRUM(true);
    float snr = synchro->getSNRfromSPECTRUM();
    EXPECT_EQ(expectedResults[4], snr) << "wrong minusInfSNR";
}
//#endif
