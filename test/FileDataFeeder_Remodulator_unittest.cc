/*#include "DataFeeder/file_data_feeder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include "../src/sdrdab/DataFeeder/file_data_feeder.h"

class RemodulatorTest: public testing::Test {
    public:
        RemodulatorTest(){
            data_1_loaded = false;
            data_2_loaded = false;
            mydatafeeder = NULL;
            our_buffer = new float[1024];
            remodulator_exp_res = new float[1024];
            fc_shift = 30000.0;
        };
        virtual ~RemodulatorTest() {
            delete[] our_buffer;
            delete[] remodulator_exp_res;
        };
        virtual void SetUp() {
            mydatafeeder = new FileDataFeeder("whatever.exe.dll",2048,2048000,229072000, 8);
            data_1_loaded = MatlabIO::ReadData(our_buffer, "./data/ut/remodulator_input.txt", 1024);
            data_2_loaded = MatlabIO::ReadData(remodulator_exp_res, "./data/ut/remodulator_expected_results.txt", 1024);
        };
        virtual void TearDown() {
            delete mydatafeeder;
        };
        float *our_buffer;
        float *remodulator_exp_res;
        float fc_shift;
        bool data_1_loaded, data_2_loaded;
        FileDataFeeder *mydatafeeder;
};

TEST_F(RemodulatorTest, data_correct_test) {
    ASSERT_TRUE(data_1_loaded);
    ASSERT_TRUE(data_2_loaded);
    double abs_err = 0.0;
    mydatafeeder->Remodulate(our_buffer,1024,fc_shift);
    for (int i=0;i<1024;i++){
        EXPECT_NEAR(remodulator_exp_res[i],our_buffer[i],0.001) << "WRONG\n!";
        abs_err+=abs(remodulator_exp_res[i]-our_buffer[i])/1024;
    }
    std::cout<< "Absolute mean error: " << abs_err << std::endl;
};

*/;