/*
 * fft_engine_unittest.cc
 *
 *  Created on: 22 lut 2015
 *      Author: kwant
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 */


#ifdef GOOGLE_UNIT_TEST

#include "fft_engine.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class FFTEngineTest  : public ::testing::Test{
    protected:
        FFTEngineTest(){

            length = 4096;

            // fft test

            fft_data_loaded = false;
            fft_result_loaded = false;

            fft_data = new float[length*2];
            fft_expected_results = new float[length*2];

            // ifft test

            ifft_data_loaded = false;
            ifft_result_loaded = false;

            ifft_data = new float[length*2];
            ifft_expected_results = new float[length*2];

            // --
            fft = new FFTEngine(length);    // possible initialization
        }

        virtual ~FFTEngineTest(){
            delete fft;

            delete [] fft_data;
            delete [] fft_expected_results;

            delete [] ifft_data;
            delete [] ifft_expected_results;
        }

        virtual void SetUp(){

            // fft test
            fft_data_loaded = MatlabIO::ReadData(fft_data, "./data/ut/fft_engine_vol1_input.txt", length*2 );
            fft_result_loaded = MatlabIO::ReadData(fft_expected_results, "./data/ut/fft_engine_vol1_expectedOutput.txt", length*2 );

            // ifft test
            ifft_data_loaded = MatlabIO::ReadData(ifft_data, "./data/ut/fft_engine_ifft_input.txt", length*2 );
            ifft_result_loaded = MatlabIO::ReadData(ifft_expected_results, "./data/ut/fft_engine_ifft_expectedOutput.txt", length*2 );
        }

        size_t length;

        // fft test
        float* fft_data;
        float* fft_expected_results;
        bool fft_data_loaded;
        bool fft_result_loaded;

        // ifft test

        float* ifft_data;
        float* ifft_expected_results;
        bool ifft_data_loaded;
        bool ifft_result_loaded;

        // --

        FFTEngine *fft;
};


TEST_F(FFTEngineTest, fft_4096) {
    ASSERT_TRUE(fft_data_loaded) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(fft_result_loaded)  << "TESTING CODE FAILED... could not load expected result";

    fft->FFT(fft_data); // inplace fft, assuming interleaved data: REAL, IMAGE

    size_t counter = 0;
    for(size_t i=0; i<length*2; ++i){
        EXPECT_NEAR(fft_data[i],fft_expected_results[i], 1e-4) << "vectors differ "<< fft_data[i] << " vs " << fft_expected_results[i] << " at index " << i << ", " << ++counter << " fails.";

        if(counter>=10){
            FAIL() << "TESTING ABORTED due to massive vector difference";
        }
    }

    // TODO add different data, eg. constant, real only, etc...
    // TODO test precision of FFT: absolute precision is not necessary,
}

TEST_F(FFTEngineTest, ifft_4096) {
    ASSERT_TRUE(ifft_data_loaded) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(ifft_result_loaded)  << "TESTING CODE FAILED... could not load expected result";

    fft->IFFT(ifft_data);

    size_t counter = 0;
    for(size_t i=0; i<length*2; ++i){
        EXPECT_NEAR(ifft_expected_results[i], ifft_data[i], 1e-7) << "vectors differ "<< ifft_data[i] << " vs " << ifft_expected_results[i] << " at index " << i << ", " << ++counter << " fails.";

        if(counter>=10){
            FAIL() << "TESTING ABORTED due to massive vector difference";
        }
    }
}

#endif
