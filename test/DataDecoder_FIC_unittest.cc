#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class FICDecTest  : public ::testing::Test{
    protected:
        FICDecTest(){
            ModeParameters param_mode_1 = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 4, 5, 3, 3096, 768, 199264, 9216, 48, 18, DAB_MODE_I};
            ModeParameters param_mode_2 = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 1, 5, 3, 3096, 768, 199264, 2304, 48, 18, DAB_MODE_II};
            ModeParameters param_mode_3 = {63, 256, 319, 153, 345, 49152, 192, 8, 4, 1, 17, 4, 4120, 1024, 49497, 3072, 6, 144, DAB_MODE_III};
            ModeParameters param_mode_4 = {252, 1024, 1276, 76, 1328, 98304, 768, 3, 6, 2, 9, 3, 3096, 768, 199264, 4608, 24, 18, DAB_MODE_IV};

            inputlength_mode_1 = 138240;
            outputlength_mode_1 = 5760;
            inputlength_mode_2 = 34560;
            outputlength_mode_2 = 1440;
            inputlength_mode_3 = 46080;
            outputlength_mode_3 = 1920;
            inputlength_mode_4 = 69120;
            outputlength_mode_4 = 2880;

            expectedinputdata_mode_1 = new float[inputlength_mode_1];
            expectedoutputdata_mode_1 = new uint8_t[outputlength_mode_1];
            expectedinputdata_mode_2 = new float[inputlength_mode_2];
            expectedoutputdata_mode_2 = new uint8_t[outputlength_mode_2];
            expectedinputdata_mode_3 = new float[inputlength_mode_3];
            expectedoutputdata_mode_3 = new uint8_t[outputlength_mode_3];
            expectedinputdata_mode_4 = new float[inputlength_mode_4];
            expectedoutputdata_mode_4 = new uint8_t[outputlength_mode_4];

            inputdataLoaded_mode_1 = false;
            outputdataLoaded_mode_1 = false;
            inputdataLoaded_mode_2 = false;
            outputdataLoaded_mode_2 = false;
            inputdataLoaded_mode_3 = false;
            outputdataLoaded_mode_3 = false;
            inputdataLoaded_mode_4 = false;
            outputdataLoaded_mode_4 = false;

            datadecoder_mode_1 = new DataDecoder(&param_mode_1);
            datadecoder_mode_2 = new DataDecoder(&param_mode_2);
            datadecoder_mode_3 = new DataDecoder(&param_mode_3);
            datadecoder_mode_4 = new DataDecoder(&param_mode_4);

        }

        virtual ~FICDecTest(){
            delete datadecoder_mode_1;
            delete [] expectedinputdata_mode_1;
            delete [] expectedoutputdata_mode_1;
            delete datadecoder_mode_2;
            delete [] expectedinputdata_mode_2;
            delete [] expectedoutputdata_mode_2;
            delete datadecoder_mode_3;
            delete [] expectedinputdata_mode_3;
            delete [] expectedoutputdata_mode_3;
            delete datadecoder_mode_4;
            delete [] expectedinputdata_mode_4;
            delete [] expectedoutputdata_mode_4;
        }

        virtual void SetUp(){
            inputdataLoaded_mode_1 = MatlabIO::ReadData(expectedinputdata_mode_1, "./data/ut/ficdec_in_mode_1.txt", inputlength_mode_1);
            outputdataLoaded_mode_1 = MatlabIO::ReadData(expectedoutputdata_mode_1, "./data/ut/ficdec_out_mode_1.txt", outputlength_mode_1);
            inputdataLoaded_mode_2 = MatlabIO::ReadData(expectedinputdata_mode_2, "./data/ut/ficdec_in_mode_2.txt", inputlength_mode_2);
            outputdataLoaded_mode_2 = MatlabIO::ReadData(expectedoutputdata_mode_2, "./data/ut/ficdec_out_mode_2.txt", outputlength_mode_2);
            inputdataLoaded_mode_3 = MatlabIO::ReadData(expectedinputdata_mode_3, "./data/ut/ficdec_in_mode_3.txt", inputlength_mode_3);
            outputdataLoaded_mode_3 = MatlabIO::ReadData(expectedoutputdata_mode_3, "./data/ut/ficdec_out_mode_3.txt", outputlength_mode_3);
            inputdataLoaded_mode_4 = MatlabIO::ReadData(expectedinputdata_mode_4, "./data/ut/ficdec_in_mode_4.txt", inputlength_mode_4);
            outputdataLoaded_mode_4 = MatlabIO::ReadData(expectedoutputdata_mode_4, "./data/ut/ficdec_out_mode_4.txt", outputlength_mode_4);

        }

        size_t inputlength_mode_1;
        size_t outputlength_mode_1;
        size_t inputlength_mode_2;
        size_t outputlength_mode_2;
        size_t inputlength_mode_3;
        size_t outputlength_mode_3;
        size_t inputlength_mode_4;
        size_t outputlength_mode_4;

        bool inputdataLoaded_mode_1;
        bool outputdataLoaded_mode_1;
        bool inputdataLoaded_mode_2;
        bool outputdataLoaded_mode_2;
        bool inputdataLoaded_mode_3;
        bool outputdataLoaded_mode_3;
        bool inputdataLoaded_mode_4;
        bool outputdataLoaded_mode_4;

        float* expectedinputdata_mode_1;
        uint8_t* expectedoutputdata_mode_1;
        float* expectedinputdata_mode_2;
        uint8_t* expectedoutputdata_mode_2;
        float* expectedinputdata_mode_3;
        uint8_t* expectedoutputdata_mode_3;
        float* expectedinputdata_mode_4;
        uint8_t* expectedoutputdata_mode_4;

        DataDecoder * datadecoder_mode_1;
        DataDecoder * datadecoder_mode_2;
        DataDecoder * datadecoder_mode_3;
        DataDecoder * datadecoder_mode_4;
};

TEST_F(FICDecTest, FICDecTest_1) {
    ASSERT_TRUE(inputdataLoaded_mode_1) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_mode_1) << "TESTING CODE FAILED... could not load output data";

    for (size_t j = 0; j < 15; j++)
    {
        datadecoder_mode_1->FICDecoder(expectedinputdata_mode_1+9216*j);
        for (size_t i = 0; i < outputlength_mode_1/15; i++)
        {
            EXPECT_EQ(expectedoutputdata_mode_1[i+j*384], datadecoder_mode_1->energ_disp_out_[i]);
        }
    }

}

TEST_F(FICDecTest, FICDecTest_2) {
    ASSERT_TRUE(inputdataLoaded_mode_2) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_mode_2) << "TESTING CODE FAILED... could not load output data";

    for (size_t j = 0; j < 15; j++)
    {
        datadecoder_mode_2->FICDecoder(expectedinputdata_mode_2+2304*j);
        for (size_t i = 0; i < outputlength_mode_2/15; i++)
        {
            EXPECT_EQ(expectedoutputdata_mode_2[i+j*96], datadecoder_mode_2->energ_disp_out_[i]);
        }
    }

}

TEST_F(FICDecTest, FICDecTest_3) {
    ASSERT_TRUE(inputdataLoaded_mode_3) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_mode_3) << "TESTING CODE FAILED... could not load output data";

    for (size_t j = 0; j < 15; j++)
    {
        datadecoder_mode_3->FICDecoder(expectedinputdata_mode_3+3072*j);
        for (size_t i = 0; i < outputlength_mode_3/15; i++)
        {
            EXPECT_EQ(expectedoutputdata_mode_3[i+j*128], datadecoder_mode_3->energ_disp_out_[i]);
        }
    }

}

TEST_F(FICDecTest, FICDecTest_4) {
    ASSERT_TRUE(inputdataLoaded_mode_4) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_mode_4) << "TESTING CODE FAILED... could not load output data";

    for (size_t j = 0; j < 15; j++)
    {
        datadecoder_mode_4->FICDecoder(expectedinputdata_mode_4+4608*j);
        for (size_t i = 0; i < outputlength_mode_4/15; i++)
        {
            EXPECT_EQ(expectedoutputdata_mode_4[i+j*192], datadecoder_mode_4->energ_disp_out_[i]);
        }
    }

}
#endif
