/*
 * DataDecoder_Depuncturer_DABP_FIC_unittest.cc
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 *
 * Mateusz Ziarko mat.ziarko@gmail.com 29.06.2017
 */

#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include <iostream>
#include <fstream>

using namespace std;

class DAB_P_info {
    public:
    	char inFilename[100];
    	char outFilename[100];
    	int SubChSize;
    	int protection;
    	bool isBprotection;
	
		DAB_P_info(){

        };

	friend istream& operator>>(istream& is, DAB_P_info& dabpinfo){
		
		string line;
		getline(is, line);

		istringstream iss(line);

		iss >> dabpinfo.inFilename;
		iss >> dabpinfo.outFilename;
		iss >> dabpinfo.SubChSize;
		iss >> dabpinfo.protection;
		iss >> dabpinfo.isBprotection;

		return is;

		}
};

class DepuncturerTest  : public ::testing::Test{
    protected:
        DepuncturerTest(){
            
            param.guard_size = 504;
            param.fft_size = 2048;
            param.symbol_size = 2552;
            param.number_of_symbols = 76;
            param.null_size = 2656;
            param.frame_size = 196608;
            param.number_of_carriers = 1536;
            param.number_of_symbols_per_fic = 3;
            param.number_of_fib = 12;
                      

            param.number_of_cif = 4;
            
            
            param.number_of_deqpsk_unit_for_read = 5;
            param.number_of_fib_per_cif = 3;
            param.number_samp_after_timedep = 3096;
            param.number_samp_after_vit = 768;
            param.sync_read_size = 199264;
            param.fic_size = 9216;
            param.number_cu_per_symbol = 48;
            param.number_symbols_per_cif = 18;

            station_info.audio_kbps = 0;
            

            param.dab_mode = DAB_MODE_II;
            
            inputlength_msc_dab = 21504;
            outputlength_msc_dab = 43104;


            inputlength_fic = 9216;
            outputlength_fic = 12384;

            expectedinputdata_msc_dab = new float[inputlength_msc_dab];
            expectedoutputdata_msc_dab = new float[outputlength_msc_dab];


            expectedinputdata_fic = new float[inputlength_fic];
            expectedoutputdata_fic = new float[outputlength_fic];

            inputdata_msc_dab = new float[inputlength_msc_dab];
            outputdata_msc_dab = new float[outputlength_msc_dab];



            inputdata_fic = new float[inputlength_fic];
            outputdata_fic = new float[outputlength_fic];

            inputdataLoaded_msc_dab = false;
            outputdataLoaded_msc_dab = false;

   

            inputdataLoaded_fic = false;
            outputdataLoaded_fic = false;

            depuncturer = new DePuncturer(&station_info, &param);


        }

        virtual ~DepuncturerTest(){
            delete depuncturer;

            delete [] expectedinputdata_fic;
            delete [] expectedoutputdata_fic;

            delete [] inputdata_fic;
            delete [] outputdata_fic;
        }

        virtual void SetUp(){

            inputdataLoaded_fic = MatlabIO::ReadData(expectedinputdata_fic, "./data/ut/depuncturer_data_in_fic_16_15_8_modes_1_2_4.txt", inputlength_fic);
            outputdataLoaded_fic = MatlabIO::ReadData(expectedoutputdata_fic, "./data/ut/depuncturer_data_out_fic_16_15_8_modes_1_2_4.txt", outputlength_fic);

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

        size_t inputlength_msc_dabp;
        size_t outputlength_msc_dabp;

        size_t inputlength_fic;
        size_t outputlength_fic;

        bool inputdataLoaded_msc_dab;
        bool outputdataLoaded_msc_dab;

        bool inputdataLoaded_msc_dabp;
        bool outputdataLoaded_msc_dabp;

        bool inputdataLoaded_fic;
        bool outputdataLoaded_fic;

        float* expectedinputdata_msc_dab;
        float* expectedoutputdata_msc_dab;

        float* expectedinputdata_msc_dabp;
        float* expectedoutputdata_msc_dabp;

        float* expectedinputdata_fic;
        float* expectedoutputdata_fic;

        float* inputdata_msc_dab;
        float* outputdata_msc_dab;

        float* inputdata_msc_dabp;
        float* outputdata_msc_dabp;

        float* inputdata_fic;
        float* outputdata_fic;

        string out;
        string in;

        ModeParameters param;
        DePuncturer * depuncturer;
        stationInfo station_info;
        DAB_P_info  dabpinfo;

};


TEST_F(DepuncturerTest, DepuncturerTest_FIC) {
    ASSERT_TRUE(inputdataLoaded_fic) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_fic) << "TESTING CODE FAILED... could not load output data";

    depuncturer->DePuncturerFICInit();
    for(size_t i =0; i < 4; i++)
        depuncturer->DePuncturerProcess(expectedinputdata_fic+((inputlength_fic/4)*i), inputlength_fic/4, outputdata_fic+((outputlength_fic/4)*i), false, false);

    for (size_t i = 0; i < outputlength_fic; i++) {
        EXPECT_EQ(outputdata_fic[i], expectedoutputdata_fic[i]) << "vectors differ on " << i;
    }
}

TEST_F(DepuncturerTest, DepuncturerTest_MSC_DABP) {
    
    ifstream input_file("./data/ut/depuncturer_data_description_dabp.txt");

	for(int a=0; a < 1092; a++){

		input_file >> dabpinfo;
	 	
	 	inputlength_msc_dabp = 0;
	    outputlength_msc_dabp = 0;

		ifstream inFile(dabpinfo.inFilename);
		while (getline(inFile, in))
	        ++inputlength_msc_dabp;
	    
	    
	    ifstream outFile(dabpinfo.outFilename);
		while (getline(outFile, out))
	        ++outputlength_msc_dabp;

        expectedinputdata_msc_dabp = new float[inputlength_msc_dabp];
        expectedoutputdata_msc_dabp = new float[outputlength_msc_dabp];

        inputdata_msc_dabp = new float[inputlength_msc_dabp];
        outputdata_msc_dabp = new float[outputlength_msc_dabp];

        inputdataLoaded_msc_dabp = false;
        outputdataLoaded_msc_dabp = false;

        inputdataLoaded_msc_dabp = MatlabIO::ReadData(expectedinputdata_msc_dabp, dabpinfo.inFilename, inputlength_msc_dabp);
        outputdataLoaded_msc_dabp = MatlabIO::ReadData(expectedoutputdata_msc_dabp, dabpinfo.outFilename, outputlength_msc_dabp);

	    ASSERT_TRUE(inputdataLoaded_msc_dabp) << "TESTING CODE FAILED... could not load input data";
	    ASSERT_TRUE(outputdataLoaded_msc_dabp) << "TESTING CODE FAILED... could not load output data";

	    
	    depuncturer->DePuncturerMSCInit(dabpinfo.SubChSize, dabpinfo.protection, true, dabpinfo.isBprotection);

	    for(size_t i =0; i < 4; i++)
	    	depuncturer->DePuncturerProcess(expectedinputdata_msc_dabp+((inputlength_msc_dabp/4)*i),inputlength_msc_dabp/4,outputdata_msc_dabp+((outputlength_msc_dabp/4)*i), true, false);

	    for (size_t i = 0; i < outputlength_msc_dabp; i++) {
	        EXPECT_EQ(expectedoutputdata_msc_dabp[i], outputdata_msc_dabp[i]) << "vectors differ on " << i << dabpinfo.inFilename ;
	    }


	            delete [] expectedinputdata_msc_dabp;
	            delete [] expectedoutputdata_msc_dabp;

	            delete [] inputdata_msc_dabp;
	            delete [] outputdata_msc_dabp;
	}
}

#endif
