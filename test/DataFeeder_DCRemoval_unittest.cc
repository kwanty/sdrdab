#ifdef GOOGLE_UNIT_TEST

#include "DataFeeder/file_data_feeder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

// If fails, check if DC_LENGTH equals 5 in "DataFeeder/abstract_data_feeder.h"

class DcRemovalTest: public testing::Test {
    public:

        DcRemovalTest(){
            data_loaded1 = false;
            data_loaded2 = false;
            data_to_test = new float[16384*5];
            dcremoval_expected_result = new float[16384*5];
            mydatafeeder = new FileDataFeeder(type_uint8,"non_existing_file",2048,2048000,229072000,8, Resampler::LINEAR);
        }

        virtual ~DcRemovalTest() {
            delete[] data_to_test;
            delete[] dcremoval_expected_result;
        }

        virtual void SetUp() {
            data_loaded1 = MatlabIO::ReadData(data_to_test, "./data/ut/dcremoval_data.txt", 16384*5);
            data_loaded2 = MatlabIO::ReadData(dcremoval_expected_result, "./data/ut/dcremoval_expected_results.txt", 16384*5);
        }

        virtual void TearDown() {
            delete mydatafeeder;

        }

        float *data_to_test;
        float *dcremoval_expected_result;
        bool data_loaded1, data_loaded2;

        FileDataFeeder *mydatafeeder;
};

TEST_F(DcRemovalTest, data_correct_test) {
    //  RingBuffer<float> *myrb = new RingBuffer<float>(5);
    //  myrb->Initialize(0.0);
    //  float *data_in = data_to_test;
    //  float *data_out = dcremoval_expected_result;
    //  ASSERT_TRUE(data_loaded1);
    //  ASSERT_TRUE(data_loaded2);
    //  float abs_err = 0.0;
    //  for (int k = 0; k<5; k++){
    //      mydatafeeder->DcRemoval(data_in,16384);
    //      for (int i=0;i<16384;i++){
    //          EXPECT_NEAR(data_out[i],data_in[i],0.00001) << "1 WRONG\n!";
    ////            EXPECT_NEAR(data_out[i],data_in[i],0.00001) << "1 WRONG\n!";
    //          abs_err+=abs(data_out[i]-data_in[i]);
    //      }
    //      abs_err/=16384;
    //      if(k<4){
    //      data_out+=16384;
    //      data_in+=16384;
    //      }
    //  }
    //  std::cout<< "Absolute mean error: " << abs_err << std::endl;

}

#endif
