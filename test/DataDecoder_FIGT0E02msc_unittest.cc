#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include <iostream>
using namespace std;

class FIGTest02  : public ::testing::Test{
    protected:
        FIGTest02(){
            //FIRST SET OF DATA
            input1Loaded = false;
            output1Loaded = false;
            type1Loaded = false;
            length1 = 21;                           // TxExSize.txt
            lengthO = 5;
            input1 = new uint8_t[length1];
            expectedOutput1 = new size_t[lengthO];
            output1 = 0;
            type1 = 0;

            extractfrombitstream = new ExtractFromBitstream();
        }

        virtual ~FIGTest02(){
            delete extractfrombitstream;
            delete [] input1;
        }

        virtual void SetUp(){
            //FIRST SET OF DATA
            input1Loaded = MatlabIO::ReadData( input1, "./data/ut/T0E2Input.txt", length1 );
            output1Loaded = MatlabIO::ReadData( expectedOutput1, "./data/ut/T0E2MSCOutput.txt", lengthO);
            type1Loaded = MatlabIO::ReadData( &type1, "./data/ut/T0E2Type.txt", 1);
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


TEST_F(FIGTest02, T0E2MSCTEST) {
    ASSERT_TRUE(input1Loaded) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(output1Loaded)  << "TESTING CODE FAILED... could not load expected result";
    ASSERT_TRUE(type1Loaded)  << "TESTING CODE FAILED... could not load type";
    output1 = extractfrombitstream->ExtractDataFromFIC(input1, length1, type1);
    EXPECT_EQ(static_cast<uint8_t>(2), output1) << "EXTENSION CHECK FAILED";
    EXPECT_EQ(static_cast<uint16_t>(this->expectedOutput1[0]), extractfrombitstream->MCIdata_.basic_Service_And_Service_Component.back().service_id) << "ServiceId CHECK FAILED";
    EXPECT_EQ(static_cast<bool>(this->expectedOutput1[1]), extractfrombitstream->MCIdata_.basic_Service_And_Service_Component.back().LF) << "LocalFlag CHECK FAILED";
    EXPECT_EQ(static_cast<uint8_t>(this->expectedOutput1[2]), extractfrombitstream->MCIdata_.basic_Service_And_Service_Component.back().nbr_service_component) << "NumberOfServiceComponent CHECK FAILED";

}

#endif
