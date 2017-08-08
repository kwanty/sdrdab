#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class SuperFrameTest  : public ::testing::Test{
protected:
	SuperFrameTest(){
		ModeParameters param_mode1 = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 4, 5, 3, 3096, 768, 199264, 9216, 48, 18, DAB_MODE_I};
		ModeParameters param_mode2 = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 1, 5, 3, 3096, 768, 199264, 2304, 48, 18, DAB_MODE_II};
		ModeParameters param_mode3 = {63, 256, 319, 153, 345, 49152, 192, 8, 4, 1, 17, 4, 4120, 1024, 49497, 3072, 6, 144, DAB_MODE_III};
		ModeParameters param_mode4 = {252, 1024, 1276, 76, 1328, 98304, 768, 3, 6, 2, 9, 3, 3096, 768, 99632, 4608, 24, 36, DAB_MODE_IV};

		inputlength_mode1 = 14784;
		outputlength_mode1 = 12472;
		inputlength_mode2 = 6049;
		outputlength_mode2 = 4018;
		inputlength_mode3 = 3456;
		outputlength_mode3 = 2658;
		inputlength_mode4 = 9216;
		outputlength_mode4 = 8034;

		expectedinputdata_mode1 = new uint8_t[inputlength_mode1];
		expectedoutputdata_mode1 = new uint8_t[outputlength_mode1];
		expectedinputdata_mode2 = new uint8_t[inputlength_mode2];
		expectedoutputdata_mode2 = new uint8_t[outputlength_mode2];
		expectedinputdata_mode3 = new uint8_t[inputlength_mode3];
		expectedoutputdata_mode3 = new uint8_t[outputlength_mode3];
		expectedinputdata_mode4 = new uint8_t[inputlength_mode4];
		expectedoutputdata_mode4 = new uint8_t[outputlength_mode4];

		inputdata_mode1 = new uint8_t[inputlength_mode1];
		inputdata_mode1 = expectedinputdata_mode1;
		outputdata_mode1 = new uint8_t[outputlength_mode1];

		inputdata_mode2 = new uint8_t[inputlength_mode2];
		inputdata_mode2 = expectedinputdata_mode2;
		outputdata_mode2 = new uint8_t[outputlength_mode2];

		inputdata_mode3 = new uint8_t[inputlength_mode3];
		inputdata_mode3 = expectedinputdata_mode3;
		outputdata_mode3 = new uint8_t[outputlength_mode3];

		inputdata_mode4 = new uint8_t[inputlength_mode4];
		inputdata_mode4 = expectedinputdata_mode4;
		outputdata_mode4 = new uint8_t[outputlength_mode4];

		inputdataLoaded_mode1 = false;
		outputdataLoaded_mode1 = false;
		inputdataLoaded_mode2 = false;
		outputdataLoaded_mode2 = false;
		inputdataLoaded_mode3 = false;
		outputdataLoaded_mode3 = false;
		inputdataLoaded_mode4 = false;
		outputdataLoaded_mode4 = false;

		info.audio_kbps = 96;
		info.sub_ch_size = 72;
		info.protection_level = 2;
		info.IsLong = true;
		info.ProtectionLevelTypeB = false;
		info.sub_ch_start_addr = 0;

		info2.audio_kbps = 112;
		info2.sub_ch_size = 72;
		info2.protection_level = 2;
		info2.IsLong = true;
		info2.ProtectionLevelTypeB = false;
		info2.sub_ch_start_addr = 0;

		info3.audio_kbps = 96;
		info3.sub_ch_size = 72;
		info3.protection_level = 2;
		info3.IsLong = true;
		info3.SubChannelId = 1;
		info3.ServiceId = 12322;
		info3.ProtectionLevelTypeB = false;
		info3.sub_ch_start_addr = 0;


		superframe_mode_1 = new SuperFrame(&info, &param_mode1);
		superframe_mode_2 = new SuperFrame(&info2, &param_mode2);
		superframe_mode_3 = new SuperFrame(&info3, &param_mode3);
		superframe_mode_4 = new SuperFrame(&info2, &param_mode4);

	}

	virtual ~SuperFrameTest(){
		delete superframe_mode_1;
		delete [] expectedinputdata_mode1;
		delete [] expectedoutputdata_mode1;
		delete superframe_mode_2;
		delete [] expectedinputdata_mode2;
		delete [] expectedoutputdata_mode2;
		delete superframe_mode_3;
		delete [] expectedinputdata_mode3;
		delete [] expectedoutputdata_mode3;
		delete superframe_mode_4;
		delete [] expectedinputdata_mode4;
		delete [] expectedoutputdata_mode4;
	}

	virtual void SetUp(){
		inputdataLoaded_mode1 = MatlabIO::ReadData(expectedinputdata_mode1, "./data/ut/super_frame_in_m1.txt", inputlength_mode1);
		outputdataLoaded_mode1 = MatlabIO::ReadData(expectedoutputdata_mode1, "./data/ut/super_frame_out_m1.txt", outputlength_mode1);
		inputdataLoaded_mode2 = MatlabIO::ReadData(expectedinputdata_mode2, "./data/ut/super_frame_in_m2.txt", inputlength_mode2);
		outputdataLoaded_mode2 = MatlabIO::ReadData(expectedoutputdata_mode2, "./data/ut/super_frame_out_m2.txt", outputlength_mode2);
		inputdataLoaded_mode3 = MatlabIO::ReadData(expectedinputdata_mode3, "./data/ut/super_frame_in_m3.txt", inputlength_mode3);
		outputdataLoaded_mode3 = MatlabIO::ReadData(expectedoutputdata_mode3, "./data/ut/super_frame_out_m3.txt", outputlength_mode3);
		inputdataLoaded_mode4 = MatlabIO::ReadData(expectedinputdata_mode4, "./data/ut/super_frame_in_m4.txt", inputlength_mode4);
		outputdataLoaded_mode4 = MatlabIO::ReadData(expectedoutputdata_mode4, "./data/ut/super_frame_out_m4.txt", outputlength_mode4);

	}

	size_t inputlength_mode1;
	size_t outputlength_mode1;
	size_t inputlength_mode2;
	size_t outputlength_mode2;
	size_t inputlength_mode3;
	size_t outputlength_mode3;
	size_t inputlength_mode4;
	size_t outputlength_mode4;

	bool inputdataLoaded_mode1;
	bool outputdataLoaded_mode1;
	bool inputdataLoaded_mode2;
	bool outputdataLoaded_mode2;
	bool inputdataLoaded_mode3;
	bool outputdataLoaded_mode3;
	bool inputdataLoaded_mode4;
	bool outputdataLoaded_mode4;

	uint8_t* expectedinputdata_mode1;
	uint8_t* expectedoutputdata_mode1;
	uint8_t* expectedinputdata_mode2;
	uint8_t* expectedoutputdata_mode2;
	uint8_t* expectedinputdata_mode3;
	uint8_t* expectedoutputdata_mode3;
	uint8_t* expectedinputdata_mode4;
	uint8_t* expectedoutputdata_mode4;

	uint8_t* inputdata_mode1;
	uint8_t* outputdata_mode1;
	uint8_t* inputdata_mode2;
	uint8_t* outputdata_mode2;
	uint8_t* inputdata_mode3;
	uint8_t* outputdata_mode3;
	uint8_t* inputdata_mode4;
	uint8_t* outputdata_mode4;

	SuperFrame * superframe_mode_1;
	SuperFrame * superframe_mode_2;
	SuperFrame * superframe_mode_3;
	SuperFrame * superframe_mode_4;

	stationInfo info;
	stationInfo info2;
	stationInfo info3;

};

TEST_F(SuperFrameTest, SF_mode1_no_errors) {
	ASSERT_TRUE(inputdataLoaded_mode1) << "TESTING CODE FAILED... could not load input data";
	ASSERT_TRUE(outputdataLoaded_mode1) << "TESTING CODE FAILED... could not load output data";

	superframe_mode_1->superframe_cifs_ = 8;
	superframe_mode_1->msc_info_.number_bits_per_cif = 2688;
	superframe_mode_1->station_info_->audio_kbps = 112;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i], outputdata_mode1[i]);
	}

	superframe_mode_1->superframe_cifs_ = 12;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i+1559], outputdata_mode1[i]);
	}

	superframe_mode_1->superframe_cifs_ = 4;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+3*1344, outputdata_mode1);

	superframe_mode_1->superframe_cifs_ = 8;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+3*1344, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i+2*1559], outputdata_mode1[i]);
	}

	superframe_mode_1->superframe_cifs_ = 12;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+3*1344, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i+3*1559], outputdata_mode1[i]);
	}

	superframe_mode_1->superframe_cifs_ = 16;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+3*1344, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i+4*1559], outputdata_mode1[i]);
	}

	superframe_mode_1->superframe_cifs_ = 4;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+6*1344, outputdata_mode1);

	superframe_mode_1->superframe_cifs_ = 8;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+6*1344, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i+5*1559], outputdata_mode1[i]);
	}

	superframe_mode_1->superframe_cifs_ = 12;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+6*1344, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i+5*1559], outputdata_mode1[i]);
	}

	superframe_mode_1->superframe_cifs_ = 16;

	superframe_mode_1->SuperFrameHandle(inputdata_mode1+6*1344, outputdata_mode1);

	for (size_t i = 0; i < 1559; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode1[i+6*1559], outputdata_mode1[i]);
	}

}


TEST_F(SuperFrameTest, SF_mode2_no_errors) {
	ASSERT_TRUE(inputdataLoaded_mode2) << "TESTING CODE FAILED... could not load input data";
	ASSERT_TRUE(outputdataLoaded_mode2) << "TESTING CODE FAILED... could not load output data";

	superframe_mode_2->msc_info_.number_bits_per_cif = 2304;
	superframe_mode_2->station_info_->audio_kbps = 96;
	superframe_mode_2->superframe_cifs_ = 0;

	for (size_t i = 0; i < 8; i++)
	{
		superframe_mode_2->superframe_cifs_ = i+1;
		superframe_mode_2->SuperFrameHandle(inputdata_mode2, outputdata_mode2);
	}

	for (size_t i = 0; i < 1339; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode2[i], outputdata_mode2[i]);
	}

	for (size_t i = 0; i < 5; i++)
	{
		superframe_mode_2->superframe_cifs_ = i+9;
		superframe_mode_2->SuperFrameHandle(inputdata_mode2, outputdata_mode2);
	}

	for (size_t i = 0; i < 1339; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode2[i+1339], outputdata_mode2[i]);
	}

}


TEST_F(SuperFrameTest, SF_mode3_no_errors) {
	ASSERT_TRUE(inputdataLoaded_mode3) << "TESTING CODE FAILED... could not load input data";
	ASSERT_TRUE(outputdataLoaded_mode3) << "TESTING CODE FAILED... could not load output data";

	for (size_t i = 0; i < 7; i++)
	{
		superframe_mode_3->superframe_cifs_ = i+1;
		superframe_mode_3->SuperFrameHandle(inputdata_mode3, outputdata_mode3);
	}

	for (size_t i = 0; i < 1329; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode3[i], outputdata_mode3[i]);
	}

	for (size_t i = 0; i < 5; i++)
	{
		superframe_mode_3->superframe_cifs_ = i+8;
		superframe_mode_3->SuperFrameHandle(inputdata_mode3+0*288, outputdata_mode3);
	}

	for (size_t i = 0; i < 1329; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode3[i+1329], outputdata_mode3[i]);
	}
}

TEST_F(SuperFrameTest, SF_mode4_no_errors) {
	ASSERT_TRUE(inputdataLoaded_mode4) << "TESTING CODE FAILED... could not load input data";
	ASSERT_TRUE(outputdataLoaded_mode4) << "TESTING CODE FAILED... could not load output data";

	superframe_mode_4->superframe_cifs_ = 2;

	for (size_t i = 0; i < 3; i++)
	{
		superframe_mode_4->SuperFrameHandle(inputdata_mode4, outputdata_mode4);
		superframe_mode_4->superframe_cifs_ += 2;
	}

	for (size_t i = 0; i < 1339; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode4[i], outputdata_mode4[i]);
	}

	superframe_mode_4->superframe_cifs_ = 2;

	for (size_t i = 0; i < 3; i++)
	{
		superframe_mode_4->SuperFrameHandle(inputdata_mode4+3*576, outputdata_mode4);
		superframe_mode_4->superframe_cifs_ += 2;
	}

	for (size_t i = 0; i < 1339; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode4[i+1339], outputdata_mode4[i]);
	}

	superframe_mode_4->superframe_cifs_ = 8;

	for (size_t j = 0; j < 4; j++)
	{
		superframe_mode_4->SuperFrameHandle(inputdata_mode4+6*576, outputdata_mode4);
		superframe_mode_4->superframe_cifs_ +=2;
	}

	for (size_t i = 0; i < 1339; i++)
	{
		EXPECT_EQ(expectedoutputdata_mode4[i+4*1339], outputdata_mode4[i]);
	}

}

#endif
