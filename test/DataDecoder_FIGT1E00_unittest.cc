#ifdef GOOGLE_UNIT_TEST

#include <iostream>
#include "DataDecoder/extract_from_bitstream.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class FIGTest10  : public ::testing::Test{
    protected:
        FIGTest10(){
            //FIRST SET OF DATA
            input1Loaded = false;
            output1Loaded = false;
            type1Loaded = false;
            length1 = 21;                           // TxExSize.txt
            lengthO = 19;
            input1 = new uint8_t[length1];
            expectedOutput1 = new size_t[lengthO];
            output1 = 0;
            type1 = 1;

            extractfrombitstream = new ExtractFromBitstream();
        }

        virtual ~FIGTest10(){
            delete extractfrombitstream;
            delete [] input1;
        }

        virtual void SetUp(){
            //FIRST SET OF DATA
            input1Loaded = MatlabIO::ReadData( input1, "./data/ut/T1E0Input.txt", length1 );
            output1Loaded = MatlabIO::ReadData( expectedOutput1, "./data/ut/T1E0Output.txt", lengthO);
            type1Loaded = MatlabIO::ReadData( &type1, "./data/ut/T1E0Type.txt", 1);
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


TEST_F(FIGTest10, T1E0TEST) {
    ASSERT_TRUE(input1Loaded) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(output1Loaded)  << "TESTING CODE FAILED... could not load expected result";
    ASSERT_TRUE(type1Loaded)  << "TESTING CODE FAILED... could not load type";
    output1 = extractfrombitstream->ExtractDataFromFIC(input1, length1, type1);
    cout << extractfrombitstream->Info_FIG_.labels1.back().ensemble_Label.label << endl ;
    EXPECT_EQ(static_cast<uint8_t>(0), output1) << "EXTENSION CHECK FAILED";
    EXPECT_EQ(static_cast<int>(this->expectedOutput1[0]), extractfrombitstream->Info_FIG_.labels1.back().charset) << "CHARSET CHECK FAILED";
    EXPECT_EQ(static_cast<bool>(this->expectedOutput1[1]), extractfrombitstream->Info_FIG_.labels1.back().other_ensemble) << "OE CHECK FAILED";
}

#endif
