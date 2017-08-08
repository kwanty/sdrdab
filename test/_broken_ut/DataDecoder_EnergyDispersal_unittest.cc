#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class DataDecoderTest : public ::testing::Test{
    protected:
        DataDecoderTest(){
            ficdatasize = (4 * 3 * 256) / 8;
            mscdatasize = 4 * (2694 - 6) / 8;

            FICenergyInputForCpp = new uint8_t[ficdatasize];
            FICenergyOutputFromMatlab = new uint8_t[ficdatasize];
            energyFIConeIN = new uint8_t[ficdatasize];
            energyFIConeOUT = new uint8_t[ficdatasize];
            energyFICzeroIN = new uint8_t[ficdatasize];
            energyFICzeroOUT= new uint8_t[ficdatasize];
            MSCenergyInputForCpp = new uint8_t[mscdatasize];
            MSCenergyOutputFromMatlab = new uint8_t[mscdatasize];
            energyMSConeIN = new uint8_t[mscdatasize];
            energyMSConeOUT= new uint8_t[mscdatasize];
            energyMSCzeroIN = new uint8_t[mscdatasize];
            energyMSCzeroOUT= new uint8_t[mscdatasize];

            FICenergyVector = new uint8_t[ficdatasize/4];
            MSCenergyVector= new uint8_t[mscdatasize/4];


            output1 = new uint8_t[ficdatasize];
            output2 = new uint8_t[ficdatasize];
            output3 = new uint8_t[ficdatasize];
            output4 = new uint8_t[mscdatasize];
            output5 = new uint8_t[mscdatasize];
            output6 = new uint8_t[mscdatasize];
            output7 = new uint8_t[ficdatasize/4];
            output8 = new uint8_t[mscdatasize/4];

            inputdataLoaded1 = false;
            outputdataLoaded1 = false;
            inputdataLoaded2 = false;
            outputdataLoaded2 =false;
            inputdataLoaded3 = false;
            outputdataLoaded3 = false;
            inputdataLoaded4 = false;
            outputdataLoaded4 = false;
            inputdataLoaded5 = false;
            outputdataLoaded5 = false;
            inputdataLoaded6 = false;
            outputdataLoaded6 = false;
            inputdataLoaded7 = false;
            outputdataLoaded7 = false;


            ModeParameters param = {504, 2048, 2552, 76, 2656, 196608, 1536, 3, 12, 4, 5, 3, 3096, 768, 199264, 9216, 48, 18, DAB_MODE_I};
            //param.fic_size = 4608;
            datadecoder = new DataDecoder(&param);


            datadecoder->energy_gen_data_msc_ = new uint8_t[(2694 - 6) / 8];
            datadecoder->EnergyDispersalInit(datadecoder->energy_gen_data_msc_, (2694 - 6));
        }

        virtual ~DataDecoderTest(){
            delete datadecoder;
            delete [] output1;
            delete [] output2;
            delete [] output3;
            delete [] output4;
            delete [] output5;
            delete [] output6;
            delete [] output7;
            delete [] output8;
            delete [] FICenergyInputForCpp;
            delete [] FICenergyOutputFromMatlab;
            delete [] energyFIConeIN;
            delete [] energyFIConeOUT;
            delete [] energyFICzeroIN;
            delete [] energyFICzeroOUT;
            delete [] MSCenergyInputForCpp;
            delete [] MSCenergyOutputFromMatlab;
            delete [] energyMSConeIN;
            delete [] energyMSConeOUT;
            delete [] energyMSCzeroIN;
            delete [] energyMSCzeroOUT;

            delete [] FICenergyVector;
            delete [] MSCenergyVector;
        }

        virtual void SetUp(){
            //gen data msc vec
            //fic variable size
            inputdataLoaded1 = MatlabIO::ReadData(ficdatasize, "./data/ut/FICenergyInputForCpp.txt", FICenergyInputForCpp);
            outputdataLoaded1 = MatlabIO::ReadData(ficdatasize, "./data/ut/FICenergyOutputFromMatlab.txt", FICenergyOutputFromMatlab);
            //msc variable size
            inputdataLoaded2 = MatlabIO::ReadData(mscdatasize, "./data/ut/MSCenergyInputForCpp.txt", MSCenergyInputForCpp);
            outputdataLoaded2 = MatlabIO::ReadData(mscdatasize, "./data/ut/MSCenergyOutputFromMatlab.txt", MSCenergyOutputFromMatlab);
            //fic one
            inputdataLoaded3 = MatlabIO::ReadData(ficdatasize, "./data/ut/energyFIConeIN.txt", energyFIConeIN);
            outputdataLoaded3 = MatlabIO::ReadData(ficdatasize, "./data/ut/energyFIConeOUT.txt", energyFIConeOUT);
            //fic zero
            inputdataLoaded4 = MatlabIO::ReadData(ficdatasize, "./data/ut/energyFICzeroIN.txt", energyFICzeroIN);
            outputdataLoaded4 = MatlabIO::ReadData(ficdatasize, "./data/ut/energyFICzeroOUT.txt", energyFICzeroOUT);
            //msc one
            inputdataLoaded5 = MatlabIO::ReadData(mscdatasize, "./data/ut/energyMSConeIN.txt", energyMSConeIN);
            outputdataLoaded5 = MatlabIO::ReadData(mscdatasize, "./data/ut/energyMSConeOUT.txt", energyMSConeOUT);
            //msc zero
            inputdataLoaded6 = MatlabIO::ReadData(mscdatasize, "./data/ut/energyMSCzeroIN.txt", energyMSCzeroIN);
            outputdataLoaded6 = MatlabIO::ReadData(mscdatasize, "./data/ut/energyMSCzeroOUT.txt", energyMSCzeroOUT);
            //gen data vectors fic/msc
            inputdataLoaded7 = MatlabIO::ReadData(ficdatasize/4, "./data/ut/FICenergyVector.txt", FICenergyVector);
            outputdataLoaded7 = MatlabIO::ReadData(mscdatasize/4, "./data/ut/MSCenergyVector.txt", MSCenergyVector);

        }

        bool inputdataLoaded1;
        bool outputdataLoaded1;
        bool inputdataLoaded2;
        bool outputdataLoaded2;
        bool inputdataLoaded3;
        bool outputdataLoaded3;
        bool inputdataLoaded4;
        bool outputdataLoaded4;
        bool inputdataLoaded5;
        bool outputdataLoaded5;
        bool inputdataLoaded6;
        bool outputdataLoaded6;
        bool inputdataLoaded7;
        bool outputdataLoaded7;


        DataDecoder * datadecoder;
        size_t ficdatasize;
        size_t mscdatasize;
        uint8_t* output1;
        uint8_t* output2;
        uint8_t* output3;
        uint8_t* output4;
        uint8_t* output5;
        uint8_t* output6;
        uint8_t* output7;
        uint8_t* output8;

        uint8_t* FICenergyInputForCpp;
        uint8_t* FICenergyOutputFromMatlab;
        uint8_t* energyFIConeIN;
        uint8_t* energyFIConeOUT;
        uint8_t* energyFICzeroIN;
        uint8_t* energyFICzeroOUT;

        uint8_t* MSCenergyInputForCpp;
        uint8_t* MSCenergyOutputFromMatlab;
        uint8_t* energyMSConeIN;
        uint8_t* energyMSConeOUT;
        uint8_t* energyMSCzeroIN;
        uint8_t* energyMSCzeroOUT;

        uint8_t* FICenergyVector;
        uint8_t* MSCenergyVector;

};


//FIC TESTS
TEST_F(DataDecoderTest, EnergyDisp_FIC_variable_size_vector) {
    ASSERT_TRUE(inputdataLoaded1) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded1) << "TESTING CODE FAILED... could not load output data";


    datadecoder->EnergyDispersalProcess(FICenergyInputForCpp, datadecoder->energy_gen_data_fic_, output1, ficdatasize/4 * 8);
    for (size_t i = 0; i < ficdatasize; i++) {
        EXPECT_EQ(output1[i], FICenergyOutputFromMatlab[i]) << "vectors differ "<< i;
    }
}

TEST_F(DataDecoderTest, EnergyDisp_FIC_const_ONES_vector) {
    ASSERT_TRUE(inputdataLoaded3) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded3) << "TESTING CODE FAILED... could not load output data";

    datadecoder->EnergyDispersalProcess(energyFIConeIN, datadecoder->energy_gen_data_fic_, output2, ficdatasize/4 * 8);
    for (size_t i = 0; i < ficdatasize; i++) {
        EXPECT_EQ(output2[i], energyFIConeOUT[i]) << "vectors differ "<< i;
    }
}

TEST_F(DataDecoderTest, EnergyDisp_FIC_const_ZEROS_vector) {
    ASSERT_TRUE(inputdataLoaded4) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded4) << "TESTING CODE FAILED... could not load output data";

    datadecoder->EnergyDispersalProcess(energyFICzeroIN, datadecoder->energy_gen_data_fic_, output3, ficdatasize/4 * 8);
    for (size_t i = 0; i < ficdatasize; i++) {
        EXPECT_EQ(output3[i], energyFICzeroOUT[i]) << "vectors differ "<< i;
    }

}

TEST_F(DataDecoderTest, EnergyDispGEN_FIC_VECTOR) {
    ASSERT_TRUE(inputdataLoaded7) << "TESTING CODE FAILED... could not load input data";

    for (size_t i = 0; i < ficdatasize/4; i++) {
        EXPECT_EQ(datadecoder->energy_gen_data_fic_[i], FICenergyVector[i]) << "vectors differ "<< i;
    }

}

//MSC
TEST_F(DataDecoderTest, EnergyDisp_MSC_variable_size_vector) {
    ASSERT_TRUE(inputdataLoaded2) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded2) << "TESTING CODE FAILED... could not load output data";

    datadecoder->EnergyDispersalProcess(MSCenergyInputForCpp, datadecoder->energy_gen_data_msc_, output4, mscdatasize/4 * 8);
    for (size_t i = 0; i < mscdatasize; i++) {
        EXPECT_EQ(output4[i], MSCenergyOutputFromMatlab[i]) << "vectors differ "<< i;
    }
}

TEST_F(DataDecoderTest, EnergyDisp_MSC_const_ONES_vector) {
    ASSERT_TRUE(inputdataLoaded5) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded5) << "TESTING CODE FAILED... could not load output data";

    datadecoder->EnergyDispersalProcess(energyMSConeIN, datadecoder->energy_gen_data_msc_, output5, mscdatasize/4 * 8);
    for (size_t i = 0; i < mscdatasize; i++) {
        EXPECT_EQ(output5[i], energyMSConeOUT[i]) << "vectors differ "<< i;
    }
}

TEST_F(DataDecoderTest, EnergyDisp_MSC_const_ZEROS_vector) {
    ASSERT_TRUE(inputdataLoaded6) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded6) << "TESTING CODE FAILED... could not load output data";

    int doom = 0;
    datadecoder->EnergyDispersalProcess(energyMSCzeroIN, datadecoder->energy_gen_data_msc_, output6, mscdatasize/4 * 8);
    for (size_t i = 0; i < mscdatasize; i++) {
        doom++;
        //std::cout << "ONE" << (int)output6[i] << "SECOND" << (int)energyMSCzeroOUT[i] << endl;
        EXPECT_EQ(output6[i], energyMSCzeroOUT[i]) << "vectors differ "<< i;
    }
    cout << "WYNIK:" << doom;
}


TEST_F(DataDecoderTest, EnergyDispGEN_MSC_VECTOR) {
    ASSERT_TRUE(outputdataLoaded7) << "TESTING CODE FAILED... could not load input data";

    for (size_t i = 0; i < mscdatasize/4; i++) {
        EXPECT_EQ(datadecoder->energy_gen_data_msc_[i], MSCenergyVector[i]) << "vectors differ "<< i;
    }

}

#endif
