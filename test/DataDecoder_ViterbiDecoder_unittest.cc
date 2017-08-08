/*
   ============================================================================
Author      : Bułat Jarosław, Babiuch Michal
E-mail      : kwant@agh.edu,pl, babiuch.michal@gmail.com
Date        : 23.05.2015
============================================================================
*/
#ifdef GOOGLE_UNIT_TEST
#include <vector>

#include "data_decoder.h"
#include "myTimer.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class DataDecoder_Viterbi  : public ::testing::Test{
    protected:
        DataDecoder_Viterbi(){
            FIClength = 3096;
            FICresultLength = 768;      //768/8
            MSClength = 10776;
            MSCresultLength = 2688;     //2688/8=336

            ficIn1 = new float[FIClength];
            ficIn2 = new float[FIClength];
            ficIn3 = new float[FIClength];
            ficIn4 = new float[FIClength];
            ficOut1 = new float[FICresultLength];
            ficOut2 = new float[FICresultLength];
            ficOut3 = new float[FICresultLength];
            ficOut4 = new float[FICresultLength];

            mscIn1 = new float[MSClength];
            mscIn2 = new float[MSClength];
            mscIn3 = new float[MSClength];
            mscIn4 = new float[MSClength];
            mscOut1 = new float[MSCresultLength];
            mscOut2 = new float[MSCresultLength];
            mscOut3 = new float[MSCresultLength];
            mscOut4 = new float[MSCresultLength];

            loaded = true;
            deviterbi = new DeViterbi();
            deviterbi->DeViterbiInit();
        }

        virtual ~DataDecoder_Viterbi(){
            delete [] ficIn1;
            delete [] ficIn2;
            delete [] ficIn3;
            delete [] ficIn4;
            delete [] ficOut1;
            delete [] ficOut2;
            delete [] ficOut3;
            delete [] ficOut4;

            delete [] mscIn1;
            delete [] mscIn2;
            delete [] mscIn3;
            delete [] mscIn4;
            delete [] mscOut1;
            delete [] mscOut2;
            delete [] mscOut3;
            delete [] mscOut4;

            delete deviterbi;
        }

        virtual void SetUp(){
            loaded &= MatlabIO::ReadData(ficIn1, "./data/ut/vitterbi_in_FIC_15_0.txt", FIClength);
            loaded &= MatlabIO::ReadData(ficIn2, "./data/ut/vitterbi_in_FIC_15_1.txt", FIClength);
            loaded &= MatlabIO::ReadData(ficIn3, "./data/ut/vitterbi_in_FIC_15_2.txt", FIClength);
            loaded &= MatlabIO::ReadData(ficIn4, "./data/ut/vitterbi_in_FIC_15_3.txt", FIClength);

            loaded &= MatlabIO::ReadData(ficOut1, "./data/ut/vitterbi_out_FIC_15_0.txt", FICresultLength);
            loaded &= MatlabIO::ReadData(ficOut2, "./data/ut/vitterbi_out_FIC_15_1.txt", FICresultLength);
            loaded &= MatlabIO::ReadData(ficOut3, "./data/ut/vitterbi_out_FIC_15_2.txt", FICresultLength);
            loaded &= MatlabIO::ReadData(ficOut4, "./data/ut/vitterbi_out_FIC_15_3.txt", FICresultLength);

            loaded &= MatlabIO::ReadData(mscIn1, "./data/ut/vitterbi_in_MSC_size10776_10_0.txt", MSClength);
            loaded &= MatlabIO::ReadData(mscIn2, "./data/ut/vitterbi_in_MSC_size10776_10_1.txt", MSClength);
            loaded &= MatlabIO::ReadData(mscIn3, "./data/ut/vitterbi_in_MSC_size10776_10_2.txt", MSClength);
            loaded &= MatlabIO::ReadData(mscIn4, "./data/ut/vitterbi_in_MSC_size10776_10_3.txt", MSClength);

            loaded &= MatlabIO::ReadData(mscOut1, "./data/ut/vitterbi_out_MSC_size2688_10_0.txt", MSCresultLength);
            loaded &= MatlabIO::ReadData(mscOut2, "./data/ut/vitterbi_out_MSC_size2688_10_1.txt", MSCresultLength);
            loaded &= MatlabIO::ReadData(mscOut3, "./data/ut/vitterbi_out_MSC_size2688_10_2.txt", MSCresultLength);
            loaded &= MatlabIO::ReadData(mscOut4, "./data/ut/vitterbi_out_MSC_size2688_10_3.txt", MSCresultLength);
        }

        size_t FIClength;
        size_t FICresultLength;
        size_t MSClength;
        size_t MSCresultLength;

        float *ficIn1, *ficIn2, *ficIn3, *ficIn4;
        float *ficOut1, *ficOut2, *ficOut3, *ficOut4;
        float *mscIn1, *mscIn2, *mscIn3, *mscIn4;
        float *mscOut1, *mscOut2, *mscOut3, *mscOut4;
        bool loaded;

        DeViterbi *deviterbi;
};


TEST_F(DataDecoder_Viterbi, FIC1) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t ficResults [FICresultLength/8];         //774*x

    float *ficOut = ficOut1;
    deviterbi-> DeViterbiProcess(ficIn1, FIClength, ficResults);
    for (size_t i=0;i<FICresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(ficOut[i+0])<<7;
        byte |= static_cast<uint8_t>(ficOut[i+1])<<6;
        byte |= static_cast<uint8_t>(ficOut[i+2])<<5;
        byte |= static_cast<uint8_t>(ficOut[i+3])<<4;
        byte |= static_cast<uint8_t>(ficOut[i+4])<<3;
        byte |= static_cast<uint8_t>(ficOut[i+5])<<2;
        byte |= static_cast<uint8_t>(ficOut[i+6])<<1;
        byte |= static_cast<uint8_t>(ficOut[i+7])<<0;

        EXPECT_EQ(ficResults[i/8], byte) << "FIC Viterbi decoding differ on position " << i;
    }
}

TEST_F(DataDecoder_Viterbi, FIC2) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t ficResults [FICresultLength/8];         //774*x

    float *ficOut = ficOut2;
    deviterbi-> DeViterbiProcess(ficIn2, FIClength, ficResults);
    for (size_t i=0;i<FICresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(ficOut[i+0])<<7;
        byte |= static_cast<uint8_t>(ficOut[i+1])<<6;
        byte |= static_cast<uint8_t>(ficOut[i+2])<<5;
        byte |= static_cast<uint8_t>(ficOut[i+3])<<4;
        byte |= static_cast<uint8_t>(ficOut[i+4])<<3;
        byte |= static_cast<uint8_t>(ficOut[i+5])<<2;
        byte |= static_cast<uint8_t>(ficOut[i+6])<<1;
        byte |= static_cast<uint8_t>(ficOut[i+7])<<0;

        EXPECT_EQ(ficResults[i/8], byte) << "FIC Viterbi decoding differ on position " << i;
    }
}

TEST_F(DataDecoder_Viterbi, FIC3) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t ficResults [FICresultLength/8];         //774*x

    float *ficOut = ficOut3;
    deviterbi-> DeViterbiProcess(ficIn3, FIClength, ficResults);
    for (size_t i=0;i<FICresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(ficOut[i+0])<<7;
        byte |= static_cast<uint8_t>(ficOut[i+1])<<6;
        byte |= static_cast<uint8_t>(ficOut[i+2])<<5;
        byte |= static_cast<uint8_t>(ficOut[i+3])<<4;
        byte |= static_cast<uint8_t>(ficOut[i+4])<<3;
        byte |= static_cast<uint8_t>(ficOut[i+5])<<2;
        byte |= static_cast<uint8_t>(ficOut[i+6])<<1;
        byte |= static_cast<uint8_t>(ficOut[i+7])<<0;

        EXPECT_EQ(ficResults[i/8], byte) << "FIC Viterbi decoding differ on position " << i;
    }
}


TEST_F(DataDecoder_Viterbi, FIC4) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t ficResults [FICresultLength/8];         //774*x

    float *ficOut = ficOut4;
    deviterbi-> DeViterbiProcess(ficIn4, FIClength, ficResults);
    for (size_t i=0;i<FICresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(ficOut[i+0])<<7;
        byte |= static_cast<uint8_t>(ficOut[i+1])<<6;
        byte |= static_cast<uint8_t>(ficOut[i+2])<<5;
        byte |= static_cast<uint8_t>(ficOut[i+3])<<4;
        byte |= static_cast<uint8_t>(ficOut[i+4])<<3;
        byte |= static_cast<uint8_t>(ficOut[i+5])<<2;
        byte |= static_cast<uint8_t>(ficOut[i+6])<<1;
        byte |= static_cast<uint8_t>(ficOut[i+7])<<0;

        EXPECT_EQ(ficResults[i/8], byte) << "FIC Viterbi decoding differ on position " << i;
    }
}

TEST_F(DataDecoder_Viterbi, MSC1) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t mscResults [MSCresultLength/8];

    float *mscOut = mscOut1;
    deviterbi-> DeViterbiProcess(mscIn1, MSClength, mscResults);
    for (size_t i=0;i<MSCresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(mscOut[i+0])<<7;
        byte |= static_cast<uint8_t>(mscOut[i+1])<<6;
        byte |= static_cast<uint8_t>(mscOut[i+2])<<5;
        byte |= static_cast<uint8_t>(mscOut[i+3])<<4;
        byte |= static_cast<uint8_t>(mscOut[i+4])<<3;
        byte |= static_cast<uint8_t>(mscOut[i+5])<<2;
        byte |= static_cast<uint8_t>(mscOut[i+6])<<1;
        byte |= static_cast<uint8_t>(mscOut[i+7])<<0;

        EXPECT_EQ(mscResults[i/8], byte) << "MSC Viterbi decoding differ on position " << i;
    }
}

TEST_F(DataDecoder_Viterbi, MSC2) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t mscResults [MSCresultLength/8];

    float *mscOut = mscOut2;
    deviterbi-> DeViterbiProcess(mscIn2, MSClength, mscResults);
    for (size_t i=0;i<MSCresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(mscOut[i+0])<<7;
        byte |= static_cast<uint8_t>(mscOut[i+1])<<6;
        byte |= static_cast<uint8_t>(mscOut[i+2])<<5;
        byte |= static_cast<uint8_t>(mscOut[i+3])<<4;
        byte |= static_cast<uint8_t>(mscOut[i+4])<<3;
        byte |= static_cast<uint8_t>(mscOut[i+5])<<2;
        byte |= static_cast<uint8_t>(mscOut[i+6])<<1;
        byte |= static_cast<uint8_t>(mscOut[i+7])<<0;

        EXPECT_EQ(mscResults[i/8], byte) << "MSC Viterbi decoding differ on position " << i << " " << (int)mscResults[i/8] << " " << (int) byte;
    }
}

TEST_F(DataDecoder_Viterbi, MSC3) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t mscResults [MSCresultLength/8];

    float *mscOut = mscOut3;
    deviterbi-> DeViterbiProcess(mscIn3, MSClength, mscResults);
    for (size_t i=0;i<MSCresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(mscOut[i+0])<<7;
        byte |= static_cast<uint8_t>(mscOut[i+1])<<6;
        byte |= static_cast<uint8_t>(mscOut[i+2])<<5;
        byte |= static_cast<uint8_t>(mscOut[i+3])<<4;
        byte |= static_cast<uint8_t>(mscOut[i+4])<<3;
        byte |= static_cast<uint8_t>(mscOut[i+5])<<2;
        byte |= static_cast<uint8_t>(mscOut[i+6])<<1;
        byte |= static_cast<uint8_t>(mscOut[i+7])<<0;

        EXPECT_EQ(mscResults[i/8], byte) << "MSC Viterbi decoding differ on position " << i;
    }
}

TEST_F(DataDecoder_Viterbi, MSC4) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";
    uint8_t mscResults [MSCresultLength/8];

    float *mscOut = mscOut4;
    deviterbi-> DeViterbiProcess(mscIn4, MSClength, mscResults);
    for (size_t i=0;i<MSCresultLength;i+=8){
        uint8_t byte = 0;
        byte |= static_cast<uint8_t>(mscOut[i+0])<<7;
        byte |= static_cast<uint8_t>(mscOut[i+1])<<6;
        byte |= static_cast<uint8_t>(mscOut[i+2])<<5;
        byte |= static_cast<uint8_t>(mscOut[i+3])<<4;
        byte |= static_cast<uint8_t>(mscOut[i+4])<<3;
        byte |= static_cast<uint8_t>(mscOut[i+5])<<2;
        byte |= static_cast<uint8_t>(mscOut[i+6])<<1;
        byte |= static_cast<uint8_t>(mscOut[i+7])<<0;

        EXPECT_EQ(mscResults[i/8], byte) << "MSC Viterbi decoding differ on position " << i;
    }
}

TEST_F(DataDecoder_Viterbi, measure_time) {
    ASSERT_TRUE(loaded) << "TESTING CODE FAILED... could not load one of Viterbi data";

    MyTimer mt;
    uint8_t ficResults [FICresultLength/8];
    uint8_t mscResults [MSCresultLength/8];

    //data is irrelevant, just see how fast code will perform
    int i;
    mt.tic();
    for (i=0;i<1024;i++) {
        deviterbi-> DeViterbiProcess(ficIn1, FIClength, ficResults);
        deviterbi-> DeViterbiProcess(ficIn2, FIClength, ficResults);
        deviterbi-> DeViterbiProcess(ficIn3, FIClength, ficResults);
        deviterbi-> DeViterbiProcess(ficIn4, FIClength, ficResults);
        deviterbi-> DeViterbiProcess(mscIn1, MSClength, mscResults);
        deviterbi-> DeViterbiProcess(mscIn2, MSClength, mscResults);
        deviterbi-> DeViterbiProcess(mscIn3, MSClength, mscResults);
        deviterbi-> DeViterbiProcess(mscIn4, MSClength, mscResults);
    }

    cout << "------------ Time: " << mt.tacNs() << " ns" << endl;
}

#endif
