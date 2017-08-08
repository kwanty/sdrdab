#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class MSCDecTest  : public ::testing::Test{
    protected:
        MSCDecTest(){
            inputlength_1= 510720;
            outputlength_1 = 14784;
            outputlength_time_1 = 26880;

            inputlength_2 = 397056;
            outputlength_2 = 4320;
            outputlength_mode2 = 8448;

            inputlength_3 = 360960;
            outputlength_3 = 4320;
            outputlength_mode3 = 7680;

            inputlength_4 = 428544;
            outputlength_4 = 8640;
            outputlength_mode4 = 13824;

            expectedinputdata_1 = new float[inputlength_1];
            expectedoutputdata_1 = new float[outputlength_1];

            expectedinputdata_2 = new float[inputlength_2];
            expectedoutputdata_2 = new float[outputlength_2];

            expectedinputdata_3 = new float[inputlength_3];
            expectedoutputdata_3 = new float[outputlength_3];

            expectedinputdata_4 = new float[inputlength_4];
            expectedoutputdata_4 = new float[outputlength_4];

            inputdata_mode1 = expectedinputdata_1;
            outputdata_mode1 = expectedinputdata_1;

            inputdata_mode2 = expectedinputdata_2;
            outputdata_mode2 = expectedinputdata_2;

            inputdata_mode3 = expectedinputdata_3;
            outputdata_mode3 = expectedinputdata_3;

            inputdata_mode4 = expectedinputdata_4;
            outputdata_mode4 = expectedinputdata_4;


            outputdata_1 = new uint8_t[15000];
            outputdata_2 = new uint8_t[15000];
            outputdata_3 = new uint8_t[15000];
            outputdata_4 = new uint8_t[15000];

            inputdataLoaded_1 = false;
            outputdataLoaded_1 = false;
            inputdataLoaded_2 = false;
            outputdataLoaded_2 = false;
            inputdataLoaded_3 = false;
            outputdataLoaded_3 = false;
            inputdataLoaded_4 = false;
            outputdataLoaded_4 = false;

            info_1.audio_kbps = 112;
            info_1.sub_ch_size = 84;
            info_1.protection_level = 2;
            info_1.IsLong = true;
            info_1.ProtectionLevelTypeB = false;
            info_1.sub_ch_start_addr = 0;

            info_2.audio_kbps = 112;
            info_2.sub_ch_size = 72;
            info_2.protection_level = 2;
            info_2.IsLong = true;
            info_2.ProtectionLevelTypeB = false;
            info_2.sub_ch_start_addr = 0;

            info_3.audio_kbps = 0;
            info_3.sub_ch_size = 72;
            info_3.protection_level = 2;
            info_3.IsLong = true;
            info_3.SubChannelId = 1;
            info_3.ServiceId = 12322;
            info_3.ProtectionLevelTypeB = false;
            info_3.sub_ch_start_addr = 0;

            info_4.audio_kbps = 0;
            info_4.sub_ch_size = 72;
            info_4.protection_level = 2;
            info_4.IsLong = true;
            info_4.SubChannelId = 1;
            info_4.ServiceId = 12817;
            info_4.ProtectionLevelTypeB = false;
            info_4.sub_ch_start_addr = 0;

            ModeParameters param_1 = {504, 2048, 2552, 76, 2656, 196608, 768, 3, 12, 4, 5, 3, 3096, 768, 199264, 2304, 48, 18, DAB_MODE_I};
            datadecoder_1 = new DataDecoder(&info_1, &param_1, DataDecoder::ALG_VITERBI_TZ);

            ModeParameters param_2 = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 1, 5, 3, 3096, 768, 199264, 2304, 48, 18, DAB_MODE_II};
            datadecoder_2 = new DataDecoder(&info_2, &param_2, DataDecoder::ALG_VITERBI_TZ);

            ModeParameters param_3 = {63, 256, 319, 153, 345, 49152, 192, 8, 4, 1, 17, 4, 4120, 1024, 49497, 3072, 6, 144, DAB_MODE_III};
            datadecoder_3 = new DataDecoder(&info_3, &param_3, DataDecoder::ALG_VITERBI_TZ);

            ModeParameters param_4 = {252, 1024, 1276, 76, 1328, 98304, 768, 3, 6, 2, 9, 3, 3096, 768, 99632, 4608, 24, 36, DAB_MODE_IV};
            datadecoder_4 = new DataDecoder(&info_4, &param_4, DataDecoder::ALG_VITERBI_TZ);
        }

        virtual ~MSCDecTest(){
            delete datadecoder_1;
            delete [] expectedinputdata_1;
            delete [] expectedoutputdata_1;
            delete [] outputdata_1;
            delete datadecoder_2;
            delete [] expectedinputdata_2;
            delete [] expectedoutputdata_2;
            delete [] outputdata_2;
            delete [] expectedinputdata_3;
            delete [] expectedoutputdata_3;
            delete [] outputdata_3;
            delete datadecoder_4;
            delete [] expectedinputdata_4;
            delete [] expectedoutputdata_4;
            delete [] outputdata_4;
        }

        virtual void SetUp(){
            inputdataLoaded_1 = MatlabIO::ReadData(expectedinputdata_1, "./data/ut/mscdec_in_m1.txt", inputlength_1);
            outputdataLoaded_1 = MatlabIO::ReadData(expectedoutputdata_1, "./data/ut/mscdec_out_m1.txt", outputlength_1);
            inputdataLoaded_2 = MatlabIO::ReadData(expectedinputdata_2, "./data/ut/mscdec_in_m2.txt", inputlength_2);
            outputdataLoaded_2 = MatlabIO::ReadData(expectedoutputdata_2, "./data/ut/mscdec_out_m2.txt", outputlength_2);
            inputdataLoaded_3 = MatlabIO::ReadData(expectedinputdata_3, "./data/ut/mscdec_in_m3.txt", inputlength_3);
            outputdataLoaded_3 = MatlabIO::ReadData(expectedoutputdata_3, "./data/ut/mscdec_out_m3.txt", outputlength_3);
            inputdataLoaded_4 = MatlabIO::ReadData(expectedinputdata_4, "./data/ut/mscdec_in_m4.txt", inputlength_4);
            outputdataLoaded_4 = MatlabIO::ReadData(expectedoutputdata_4, "./data/ut/mscdec_out_m4.txt", outputlength_4);

        }

        size_t inputlength_1;
        size_t outputlength_1;
        size_t inputlength_2;
        size_t outputlength_2;
        size_t inputlength_3;
        size_t outputlength_3;
        size_t inputlength_4;
        size_t outputlength_4;

        bool inputdataLoaded_1;
        bool outputdataLoaded_1;
        bool inputdataLoaded_2;
        bool outputdataLoaded_2;
        bool inputdataLoaded_3;
        bool outputdataLoaded_3;
        bool inputdataLoaded_4;
        bool outputdataLoaded_4;

        float* expectedinputdata_1;
        float* expectedoutputdata_1;
        float* expectedinputdata_2;
        float* expectedoutputdata_2;
        float* expectedinputdata_3;
        float* expectedoutputdata_3;
        float* expectedinputdata_4;
        float* expectedoutputdata_4;

        float* inputdata_mode1;
        float* outputdata_mode1;
        float* inputdata_mode2;
        float* outputdata_mode2;
        float* inputdata_mode3;
        float* outputdata_mode3;
        float* inputdata_mode4;
        float* outputdata_mode4;

        size_t outputlength_time_1;
        uint8_t* outputdata_1;
        size_t outputlength_mode2;
        uint8_t* outputdata_2;
        size_t outputlength_mode3;
        uint8_t* outputdata_3;
        size_t outputlength_mode4;
        uint8_t* outputdata_4;

        stationInfo info_1;
        DataDecoder * datadecoder_1;
        stationInfo info_2;
        DataDecoder * datadecoder_2;
        stationInfo info_3;
        DataDecoder * datadecoder_3;
        stationInfo info_4;
        DataDecoder * datadecoder_4;

};

TEST_F(MSCDecTest, MSCDecTest_1) {
    ASSERT_TRUE(inputdataLoaded_1) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_1) << "TESTING CODE FAILED... could not load output data";

    datadecoder_1->msc_info_.number_dab_frame = 1;
    for (size_t i = 0; i < 5; i++)
    {
        datadecoder_1->MSCDecoder(inputdata_mode1, inputlength_1, outputdata_1, true);
        datadecoder_1->msc_info_.number_dab_frame++;
        inputdata_mode1 += outputlength_time_1;
    }

    for (size_t i = 0; i < 3; i++)
    {
        datadecoder_1->MSCDecoder(inputdata_mode1, inputlength_1, outputdata_1, true);
        datadecoder_1->msc_info_.number_dab_frame++;
        inputdata_mode1 += outputlength_time_1;

        for (size_t j = 0; j < 1344; j++)
        {
            EXPECT_EQ(expectedoutputdata_1[j+i*1344], datadecoder_1->superframe_[j+i*1344]);
        }
    }

    for (size_t i = 0; i < 4; i++)
    {
        datadecoder_1->MSCDecoder(inputdata_mode1, inputlength_1, outputdata_1, true);
        datadecoder_1->msc_info_.number_dab_frame++;
        inputdata_mode1 += outputlength_time_1;

        for (size_t j = 0; j < 1344; j++)
        {
            EXPECT_EQ(expectedoutputdata_1[j+(3+i)*1344], datadecoder_1->superframe_[j+i*1344]);
        }
    }

    datadecoder_1->MSCDecoder(inputdata_mode1, inputlength_1, outputdata_1, true);
    datadecoder_1->msc_info_.number_dab_frame++;
    inputdata_mode1 += outputlength_time_1;

    for (size_t i = 0; i < 2; i++)
    {
        datadecoder_1->MSCDecoder(inputdata_mode1, inputlength_1, outputdata_1, true);
        datadecoder_1->msc_info_.number_dab_frame++;
        inputdata_mode1 += outputlength_time_1;

        for (size_t j = 0; j < 1344; j++)
        {
            EXPECT_EQ(expectedoutputdata_1[j+(8+i)*1344], datadecoder_1->superframe_[j+i*1344]);
        }
    }

}

TEST_F(MSCDecTest, MSCDecTest_2) {
    ASSERT_TRUE(inputdataLoaded_2) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_2) << "TESTING CODE FAILED... could not load output data";

    datadecoder_2->msc_info_.number_dab_frame = 1;
    for (size_t i = 0; i < 17; i++)
    {
        datadecoder_2->MSCDecoder(inputdata_mode2, inputlength_2, outputdata_2, true);
        datadecoder_2->msc_info_.number_dab_frame++;
        inputdata_mode2 += outputlength_mode2;
    }

    for (size_t i = 0; i < 288; i++)
    {
        EXPECT_EQ(expectedoutputdata_2[i], datadecoder_2->superframe_[i]);
    }

    for (size_t j = 0; j < 7; j++)
    {
        datadecoder_2->MSCDecoder(inputdata_mode2, inputlength_2, outputdata_2, true);
        datadecoder_2->msc_info_.number_dab_frame++;
        inputdata_mode2 += outputlength_mode2;

        for (size_t i = 0; i < 288; i++)
        {
            EXPECT_EQ(expectedoutputdata_2[i+(j+1)*288], datadecoder_2->superframe_[i+(j+1)*288]);
        }
    }

    for (size_t j = 0; j < 5; j++)
    {
        datadecoder_2->MSCDecoder(inputdata_mode2, inputlength_2, outputdata_2, true);
        datadecoder_2->msc_info_.number_dab_frame++;
        inputdata_mode2 += outputlength_mode2;

        for (size_t i = 0; i < 288; i++)
        {
            EXPECT_EQ(expectedoutputdata_2[i+(8+j)*288], datadecoder_2->superframe_[i+j*288]);
        }
    }

    for (size_t j = 0; j < 2; j++)
    {
        datadecoder_2->MSCDecoder(inputdata_mode2, inputlength_2, outputdata_2, true);
        datadecoder_2->msc_info_.number_dab_frame++;
        inputdata_mode2 += outputlength_mode2;

        for (size_t i = 0; i < 288; i++)
        {
            EXPECT_EQ(expectedoutputdata_2[i+(13+j)*288], datadecoder_2->superframe_[i+j*288]);
        }
    }
}

TEST_F(MSCDecTest, MSCDecTest_3) {
    ASSERT_TRUE(inputdataLoaded_3) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_3) << "TESTING CODE FAILED... could not load output data";

    datadecoder_3->msc_info_.number_dab_frame = 1;
    for (size_t i = 0; i < 17; i++)
    {
        datadecoder_3->MSCDecoder(inputdata_mode3, inputlength_3, outputdata_3, true);
        datadecoder_3->msc_info_.number_dab_frame++;
        inputdata_mode3 += outputlength_mode3;
    }

    for (size_t i = 0; i < 288; i++)
    {
        EXPECT_EQ(expectedoutputdata_3[i], datadecoder_3->superframe_[i]);
    }

    for (size_t j = 0; j < 6; j++)
    {
        datadecoder_3->MSCDecoder(inputdata_mode3, inputlength_3, outputdata_3, true);
        datadecoder_3->msc_info_.number_dab_frame++;
        inputdata_mode3 += outputlength_mode3;

        for (size_t i = 0; i < 288; i++)
        {
            EXPECT_EQ(expectedoutputdata_3[i+(j+1)*288], datadecoder_3->superframe_[i+(j+1)*288]);
        }
    }

    for (size_t j = 0; j < 5; j++)
    {
        datadecoder_3->MSCDecoder(inputdata_mode3, inputlength_3, outputdata_3, true);
        datadecoder_3->msc_info_.number_dab_frame++;
        inputdata_mode3 += outputlength_mode3;

        for (size_t i = 0; i < 288; i++)
        {
            EXPECT_EQ(expectedoutputdata_3[i+(7+j)*288], datadecoder_3->superframe_[i+j*288]);
        }
    }
}

TEST_F(MSCDecTest, MSCDecTest_4) {
    ASSERT_TRUE(inputdataLoaded_4) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_4) << "TESTING CODE FAILED... could not load output data";

    datadecoder_4->msc_info_.number_dab_frame = 1;

    for (size_t i = 0; i < 8; i++)
    {
        datadecoder_4->MSCDecoder(inputdata_mode4, inputlength_4, outputdata_4, true);
        datadecoder_4->msc_info_.number_dab_frame++;
        inputdata_mode4 += outputlength_mode4;
    }

    for (size_t j = 0; j < 3; j++)
    {
        datadecoder_4->MSCDecoder(inputdata_mode4, inputlength_4, outputdata_4, true);
        datadecoder_4->msc_info_.number_dab_frame++;
        inputdata_mode4 += outputlength_mode4;

        for (size_t i = 0; i < 576; i++)
        {
            EXPECT_EQ(expectedoutputdata_4[i+j*576], datadecoder_4->superframe_[i+j*576]);
        }
    }

    for (size_t j = 0; j < 5; j++)
    {
        datadecoder_4->MSCDecoder(inputdata_mode4, inputlength_4, outputdata_4, true);
        datadecoder_4->msc_info_.number_dab_frame++;
        inputdata_mode4 += outputlength_mode4;

        for (size_t i = 0; i < 576; i++)
        {
            EXPECT_EQ(expectedoutputdata_4[i+(3+j)*576], datadecoder_4->superframe_[i+j*576]);
        }
    }

    for (size_t j = 0; j < 5; j++)
    {
        datadecoder_4->MSCDecoder(inputdata_mode4, inputlength_4, outputdata_4, true);
        datadecoder_4->msc_info_.number_dab_frame++;
        inputdata_mode4 += outputlength_mode4;

        for (size_t i = 0; i < 576; i++)
        {
            EXPECT_EQ(expectedoutputdata_4[i+(8+j)*576], datadecoder_4->superframe_[i+j*576]);
        }
    }

    for (size_t j = 0; j < 2; j++)
    {
        datadecoder_4->MSCDecoder(inputdata_mode4, inputlength_4, outputdata_4, true);
        datadecoder_4->msc_info_.number_dab_frame++;
        inputdata_mode4 += outputlength_mode4;

        for (size_t i = 0; i < 576; i++)
        {
            EXPECT_EQ(expectedoutputdata_4[i+(13+j)*576], datadecoder_4->superframe_[i+j*576]);
        }
    }

}

#endif
