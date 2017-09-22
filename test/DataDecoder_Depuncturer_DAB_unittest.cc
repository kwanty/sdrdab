/*
 * DataDecoder_Depuncturer_DAB_unittest.cc
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 *
 * Szymon Bar szbar@student.agh.edu.pl 29.06.2017
 */

#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// class for extracting test parameters from file
class DAB_info {
public:
    char inFileName[100];
    char outFileName[100];
    int kbps;
    int protection;

    DAB_info() {};

    friend istream& operator>>(istream& is, DAB_info& dabinfo) {
        string line;
        getline(is, line);

        istringstream iss(line);
        iss >> dabinfo.inFileName;
        iss >> dabinfo.outFileName;
        iss >> dabinfo.kbps;
        iss >> dabinfo.protection;

        return is;
    }
};

class DepuncturerTest  : public ::testing::Test{
    public:
        DepuncturerTest(){
            param.number_of_cif = 4;                                        // zawsze 4

            depuncturer = new DePuncturer(&param);							// wypelniam mode_parameters_ zawartoscia param

            depuncturer->devPI_[0] = 3364391048u;
            depuncturer->devPI_[1] = 3364407432u;
            depuncturer->devPI_[2] = 3368601736u;
            depuncturer->devPI_[3] = 3368601800u;
            depuncturer->devPI_[4] = 3435710664u;
            depuncturer->devPI_[5] = 3435711688u;
            depuncturer->devPI_[6] = 3435973832u;
            depuncturer->devPI_[7] = 3435973836u;
            depuncturer->devPI_[8] = 3972844748u;
            depuncturer->devPI_[9] = 3972852940u;
            depuncturer->devPI_[10] = 3974950092u;
            depuncturer->devPI_[11] = 3974950124u;
            depuncturer->devPI_[12] = 4008504556u;
            depuncturer->devPI_[13] = 4008505068u;
            depuncturer->devPI_[14] = 4008636140u;
            depuncturer->devPI_[15] = 4008636142u;
            depuncturer->devPI_[16] = 4277071598u;
            depuncturer->devPI_[17] = 4277075694u;
            depuncturer->devPI_[18] = 4278124270u;
            depuncturer->devPI_[19] = 4278124286u;
            depuncturer->devPI_[20] = 4294901502u;
            depuncturer->devPI_[21] = 4294901758u;
            depuncturer->devPI_[22] = 4294967294u;
            depuncturer->devPI_[23] = 4294967295u;
        }

        size_t inputlength_msc_dab;
        size_t outputlength_msc_dab;

        bool inputdataLoaded_msc_dab;
        bool outputdataLoaded_msc_dab;

        float* expectedinputdata_msc_dab;
        float* expectedoutputdata_msc_dab;

        float* inputdata_msc_dab;
        float* outputdata_msc_dab;

        ModeParameters param;
        DePuncturer * depuncturer;

        string in, out;

        DAB_info dabinfo;
};

TEST_F(DepuncturerTest, DepuncturerTest_MSC_DAB) {
    ifstream input_file("./data/ut/depuncturer_data_description_dab.txt");

    for (int ii = 0; ii < 64; ii++) {
        input_file >> dabinfo;

        inputlength_msc_dab = 0;
        outputlength_msc_dab = 0;

        ifstream inFile(dabinfo.inFileName);
        while (getline(inFile, in))
            ++inputlength_msc_dab;
        
        
        ifstream outFile(dabinfo.outFileName);
        while (getline(outFile, out))
            ++outputlength_msc_dab;

        expectedinputdata_msc_dab = new float[inputlength_msc_dab];
        expectedoutputdata_msc_dab = new float[outputlength_msc_dab];

        inputdata_msc_dab = new float[inputlength_msc_dab];
        outputdata_msc_dab = new float[outputlength_msc_dab];

        inputdataLoaded_msc_dab = false;
        outputdataLoaded_msc_dab = false;

        inputdataLoaded_msc_dab = MatlabIO::ReadData(expectedinputdata_msc_dab, dabinfo.inFileName, inputlength_msc_dab);
        outputdataLoaded_msc_dab = MatlabIO::ReadData(expectedoutputdata_msc_dab, dabinfo.outFileName, outputlength_msc_dab);

        ASSERT_TRUE(inputdataLoaded_msc_dab) << "TESTING CODE FAILED... could not load input data";
        ASSERT_TRUE(outputdataLoaded_msc_dab) << "TESTING CODE FAILED... could not load output data";

        depuncturer->DePuncturerMSCInit(dabinfo.kbps, dabinfo.protection, false, false);

        for(size_t i =0; i < 4; i++)
            depuncturer->DePuncturerProcess(expectedinputdata_msc_dab+((inputlength_msc_dab/4)*i), inputlength_msc_dab/4, outputdata_msc_dab+((outputlength_msc_dab/4)*i), true, true);

        for (size_t i = 0; i < outputlength_msc_dab; i++) {
            EXPECT_EQ(expectedoutputdata_msc_dab[i], outputdata_msc_dab[i]) << "vectors differ on " << i << dabinfo.inFileName;
        }

        delete [] expectedinputdata_msc_dab;
        delete [] expectedoutputdata_msc_dab;

        delete [] inputdata_msc_dab;
        delete [] outputdata_msc_dab;
    }
    delete depuncturer;
}

#endif
