#include <stdint-gcc.h>
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include "../src/sdrdab/DataFeeder/abstract_data_feeder.h"
#include <fstream>

class MyDataFeeder: public AbstractDataFeeder {
public:
    MyDataFeeder(int number_of_bits) : AbstractDataFeeder(number_of_bits) {};
    ~MyDataFeeder() {};
    virtual void StopProcessing(void) {};
    virtual void ReadAsync(void *data_needed) {};
    virtual uint32_t GetCenterFrequency(void) { return 0; };
    virtual uint32_t GetSamplingFrequency(void) { return 2048000; };
    virtual uint32_t SetCenterFrequency(uint32_t fc) { return 0; };
    virtual uint32_t SetSamplingFrequency(uint32_t fs) { return 2048000; };
    virtual bool FromFile(void) { return false; };
    virtual bool FromDongle(void) { return false; };
    virtual void HandleDrifts(float fc_drift, float fs_drift) {};
    virtual bool EverythingOK(void) { return false; };
};

class AbstractRemodulatorTest: public testing::Test {
    public:
        AbstractRemodulatorTest(){
            mydatafeeder = NULL;
            input_buffer = new float[393216];
            expected_output_buffer = new float[393216];
        }
        virtual ~AbstractRemodulatorTest() {
            delete[] input_buffer;
            delete[] expected_output_buffer;
        }
        virtual void SetUp() {
            mydatafeeder = new MyDataFeeder(12);
        }
        virtual void TearDown() {
            delete mydatafeeder;
        }
        float *input_buffer;
        float *expected_output_buffer;
        MyDataFeeder *mydatafeeder;
};

TEST_F(AbstractRemodulatorTest, high_fc_shift) {
    bool data_0_loaded = MatlabIO::ReadData(input_buffer, "./data/ut/remodulate_input_data.txt", 393216);
    bool data_0_expected = MatlabIO::ReadData(expected_output_buffer, "./data/ut/remodulate_output_data.txt", 393216);

    ASSERT_TRUE(data_0_loaded);
    ASSERT_TRUE(data_0_expected);
    double abs_err = 0.0;
    float fc_shift = 7164.042;
    mydatafeeder->Remodulate(input_buffer, 393216, fc_shift);

    for (int i=0; i<393216; i++){
        EXPECT_NEAR(expected_output_buffer[i], input_buffer[i], 0.05) << "WRONG\n!";
        abs_err+=abs(expected_output_buffer[i] - input_buffer[i])/393216;
    }
    std::cout<< "Absolute mean error: " << abs_err << std::endl;
};

TEST_F(AbstractRemodulatorTest, low_fc_shift) {
    bool data_1_loaded = MatlabIO::ReadData(input_buffer, "./data/ut/remodulate_input_data1.txt", 393216);
    bool data_1_expected = MatlabIO::ReadData(expected_output_buffer, "./data/ut/remodulate_output_data1.txt", 393216);

    ASSERT_TRUE(data_1_loaded);
    ASSERT_TRUE(data_1_expected);
    double abs_err = 0.0;
    float fc_shift = 357.982;
    mydatafeeder->Remodulate(input_buffer, 393216, fc_shift);

    for (int i=0; i<393216; i++){
        EXPECT_NEAR(expected_output_buffer[i], input_buffer[i], 0.05) << "WRONG\n!";
        abs_err+=abs(expected_output_buffer[i] - input_buffer[i])/393216;
    }
    std::cout<< "Absolute mean error: " << abs_err << std::endl;
};

TEST_F(AbstractRemodulatorTest, zero_fc_shift) {
    bool data_2_loaded = MatlabIO::ReadData(input_buffer, "./data/ut/remodulate_input_data2.txt", 393216);
    bool data_2_expected = MatlabIO::ReadData(expected_output_buffer, "./data/ut/remodulate_output_data2.txt", 393216);

    ASSERT_TRUE(data_2_loaded);
    ASSERT_TRUE(data_2_expected);
    double abs_err = 0.0;
    float fc_shift = 0.0;
    mydatafeeder->Remodulate(input_buffer, 393216, fc_shift);

    for (int i=0; i<393216; i++){
        EXPECT_NEAR(expected_output_buffer[i], input_buffer[i], 0.05) << "WRONG\n!";
        abs_err+=abs(expected_output_buffer[i] - input_buffer[i])/393216;
    }
    std::cout<< "Absolute mean error: " << abs_err << std::endl;
};

TEST_F(AbstractRemodulatorTest, negative_low_fc_shift) {
    bool data_3_loaded = MatlabIO::ReadData(input_buffer, "./data/ut/remodulate_input_data3.txt", 393216);
    bool data_3_expected = MatlabIO::ReadData(expected_output_buffer, "./data/ut/remodulate_output_data3.txt", 393216);

    ASSERT_TRUE(data_3_loaded);
    ASSERT_TRUE(data_3_expected);
    double abs_err = 0.0;
    float fc_shift = -244.065;
    mydatafeeder->Remodulate(input_buffer, 393216, fc_shift);

    for (int i=0; i<393216; i++){
        EXPECT_NEAR(expected_output_buffer[i], input_buffer[i], 0.05) << "WRONG\n!";
        abs_err+=abs(expected_output_buffer[i] - input_buffer[i])/393216;
    }
    std::cout<< "Absolute mean error: " << abs_err << std::endl;
};

TEST_F(AbstractRemodulatorTest, negative_high_fc_shift) {
    bool data_4_loaded = MatlabIO::ReadData(input_buffer, "./data/ut/remodulate_input_data4.txt", 393216);
    bool data_4_expected = MatlabIO::ReadData(expected_output_buffer, "./data/ut/remodulate_output_data4.txt", 393216);

    ASSERT_TRUE(data_4_loaded);
    ASSERT_TRUE(data_4_expected);
    double abs_err = 0.0;
    float fc_shift = -6965.253;
    mydatafeeder->Remodulate(input_buffer, 393216, fc_shift);

    for (int i=0; i<393216; i++){
        EXPECT_NEAR(expected_output_buffer[i], input_buffer[i], 0.05) << "WRONG\n!";
        abs_err+=abs(expected_output_buffer[i] - input_buffer[i])/393216;
    }
    std::cout<< "Absolute mean error: " << abs_err << std::endl;
};
