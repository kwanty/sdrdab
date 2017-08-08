#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class FIGTest01  : public ::testing::Test{
    protected:
        FIGTest01(){
            //FIRST SET OF DATA
            input2Loaded = false;
            output1Loaded = false;
            type1Loaded = false;
            length2 = 5;
            lengthO = 4;// TxExSize.txt
            input2 = new uint8_t[length2];
            expectedOutput1 = new size_t[lengthO];
            output1 = 0;
            type1 = 0;

            extractfrombitstream = new ExtractFromBitstream();
        }

        virtual ~FIGTest01(){
            delete extractfrombitstream;
            delete [] input2;
            delete [] expectedOutput1;
        }

        virtual void SetUp(){
            //FIRST SET OF DATA
            input2Loaded = MatlabIO::ReadData( input2, "./data/ut/T0E1Input.txt", length2 );
            output1Loaded = MatlabIO::ReadData( expectedOutput1, "./data/ut/T0E1LongOutput.txt", lengthO);
            type1Loaded = MatlabIO::ReadData( &type1, "./data/ut/T0E1Type.txt", 1);
        }
        //FIRST SET OF DATA
        uint8_t length2;
        size_t lengthO;
        uint8_t* input2;
        size_t* expectedOutput1;
        uint8_t output1;
        uint8_t type1;
        bool input2Loaded;
        bool output1Loaded;
        bool type1Loaded;
        ExtractFromBitstream * extractfrombitstream;
};


TEST_F(FIGTest01, T0E1LongTEST) {
    ASSERT_TRUE(input2Loaded) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(output1Loaded)  << "TESTING CODE FAILED... could not load expected result";
    ASSERT_TRUE(type1Loaded)  << "TESTING CODE FAILED... could not load type";
    output1 = extractfrombitstream->ExtractDataFromFIC(input2, length2, type1);
    EXPECT_EQ(static_cast<uint8_t>(1), output1) << "EXTENSION CHECK FAILED";
    EXPECT_EQ(static_cast<uint8_t>(this->expectedOutput1[0]), extractfrombitstream->MCIdata_.subChannel_Basic_Information.back().subchannel_id) << "SubChannelId CHECK FAILED";
    EXPECT_EQ(static_cast<uint16_t>(this->expectedOutput1[1]), extractfrombitstream->MCIdata_.subChannel_Basic_Information.back().start_address) << "StartAddress CHECK FAILED";
    EXPECT_EQ(static_cast<uint16_t>(this->expectedOutput1[2]), extractfrombitstream->MCIdata_.subChannel_Basic_Information.back().subchannel_size) << "SubChannleSize CHECK FAILED";
    EXPECT_EQ(static_cast<uint8_t>(this->expectedOutput1[3]), extractfrombitstream->MCIdata_.subChannel_Basic_Information.back().protection_level + 1) << "ProtectionLevel CHECK FAILED";
}

#endif
