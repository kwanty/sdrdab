#ifdef GOOGLE_UNIT_TEST

#include "DataDecoder/superframe.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class CRC16  : public ::testing::Test{
    protected:
        CRC16(){
            length = 254;
            data = new uint8_t[length];

            dataLoaded = false;

            ModeParameters param = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 4, 5, 3, 3096, 768, 199264, 9216, 48, 18, DAB_MODE_I};
            superframe = new SuperFrame(&param);
        }

        virtual ~CRC16(){
            delete superframe;
            delete [] data;
        }

        virtual void SetUp(){
            dataLoaded = MatlabIO::ReadData(data, "./data/ut/CRC_input.txt", length);
        }

        size_t length;
        bool dataLoaded;
        SuperFrame * superframe;
        uint8_t* data;

};


TEST_F(CRC16, CRC16) {
    ASSERT_TRUE(dataLoaded) << "TESTING CODE FAILED... could not load expected data";
    bool result = false;
    result = superframe->CRC16(data, length);

    EXPECT_EQ(result, true) << "CRC Check FAILED.";
}

#endif

