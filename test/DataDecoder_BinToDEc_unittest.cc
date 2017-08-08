#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class BinToDec  : public ::testing::Test{
    protected:
        BinToDec(){
            ModeParameters param = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 4, 5, 3, 3096, 768, 199264, 9216, 48, 18, DAB_MODE_I};
            datadecoder = new DataDecoder(&param);
        }

        virtual ~BinToDec(){
            delete datadecoder;
        }

        virtual void SetUp(){
        }
        DataDecoder * datadecoder;
};


TEST_F(BinToDec, ShiftAndLengthFor12bitsLength) {
    uint16_t values[16]={0, 1, 2, 3, 11, 536, 1024, 0xFF, 0x0E00, 2959, 951, 1391, 684, 0, 3398, 1435 };

    for( size_t val=0; val<16; ++val ){
        for( size_t shift=0; shift<30; ++shift ){
            uint8_t data[16]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
            uint16_t value = values[val];
            size_t dataLength = 12;
            while(dataLength){
                uint8_t mask = 0xFE | (static_cast<uint8_t>(value)&0x01);   // least significant bit
                mask <<= (7-(shift+dataLength-1)%8);
                mask |= 0xFF >> (((shift+dataLength-1)%8)+1);
                data[(shift+dataLength-1)/8] &= mask;
                dataLength--;
                value >>=1;
            }
            uint16_t result = datadecoder->BinToDec(data, shift, 12);
            EXPECT_EQ(result, values[val]) << "BinToDec faild at value:" << values[val] << " shift:" << shift << " result:" << result;
        }
    }
}

#endif

