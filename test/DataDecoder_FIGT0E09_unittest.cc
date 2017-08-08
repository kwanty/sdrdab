#ifdef GOOGLE_UNIT_TEST

#include "DataDecoder/extract_from_bitstream.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class FIGTest09  : public ::testing::Test{
    protected:
        FIGTest09(){
            //FIRST SET OF DATA
            input1Loaded = false;
            output1Loaded = false;
            type1Loaded = false;
            length1 = 4;                            // TxExSize.txt
            lengthO = 3;
            input1 = new uint8_t[length1];
            expectedOutput1 = new size_t[lengthO];
            output1 = 0;
            type1 = 0;

            extractfrombitstream = new ExtractFromBitstream();
        }

        virtual ~FIGTest09(){
            delete extractfrombitstream;
            delete [] input1;
        }

        virtual void SetUp(){
            //FIRST SET OF DATA
            input1Loaded = MatlabIO::ReadData( input1, "./data/ut/T0E9Input.txt", length1 );
            output1Loaded = MatlabIO::ReadData( expectedOutput1, "./data/ut/T0E9FOutput.txt", lengthO);
            type1Loaded = MatlabIO::ReadData( &type1, "./data/ut/T0E9Type.txt", 1);
        }
        //FIRST SET OF DATA
        size_t length1;
        size_t lengthO;
        uint8_t* input1;
        size_t* expectedOutput1;
        uint8_t output1;
        uint8_t type1;
        bool input1Loaded;
        bool output1Loaded;
        bool type1Loaded;
        ExtractFromBitstream * extractfrombitstream;
};


TEST_F(FIGTest09, T0E9TEST) {
    ASSERT_TRUE(input1Loaded) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(output1Loaded)  << "TESTING CODE FAILED... could not load expected result";
    ASSERT_TRUE(type1Loaded)  << "TESTING CODE FAILED... could not load type";
    output1 = extractfrombitstream->ExtractDataFromFIC(input1, length1, type1);

    EXPECT_EQ(static_cast<uint8_t>(9), output1) << "EXTENSION CHECK FAILED";
    EXPECT_EQ((2*static_cast<uint8_t>(this->expectedOutput1[0])), extractfrombitstream->MCIdata_.country_LTO_and_International_table.ensemble_lto) << "LTO CHECK FAILED";
    EXPECT_EQ(static_cast<uint8_t>(this->expectedOutput1[1]), extractfrombitstream->MCIdata_.country_LTO_and_International_table.ensemble_ecc) << "ECC CHECK FAILED";
    EXPECT_EQ(static_cast<uint8_t>(this->expectedOutput1[2]), extractfrombitstream->MCIdata_.country_LTO_and_International_table.international_table_id) << "IntTableId CHECK FAILED";
}

#endif
