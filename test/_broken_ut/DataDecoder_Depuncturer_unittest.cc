#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

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
            param.dab_mode = DAB_MODE_I;
            inputlength_msc_dab = 24096;
            outputlength_msc_dab = 43392;

            inputlength_msc_dabp = 23808;
            outputlength_msc_dabp = 43104;

            inputlength_fic = 9216;
            outputlength_fic = 12384;

            expectedinputdata_msc_dab = new float[inputlength_msc_dab];
            expectedoutputdata_msc_dab = new float[outputlength_msc_dab];

            expectedinputdata_msc_dabp = new float[inputlength_msc_dabp];
            expectedoutputdata_msc_dabp = new float[outputlength_msc_dabp];

            expectedinputdata_fic = new float[inputlength_fic];
            expectedoutputdata_fic = new float[outputlength_fic];

            inputdata_msc_dab = new float[inputlength_msc_dab];
            outputdata_msc_dab = new float[outputlength_msc_dab];

            inputdata_msc_dabp = new float[inputlength_msc_dabp];
            outputdata_msc_dabp = new float[outputlength_msc_dabp];

            inputdata_fic = new float[inputlength_fic];
            outputdata_fic = new float[outputlength_fic];

            inputdataLoaded_msc_dab = false;
            outputdataLoaded_msc_dab = false;

            inputdataLoaded_msc_dabp = false;
            outputdataLoaded_msc_dabp = false;

            inputdataLoaded_fic = false;
            outputdataLoaded_fic = false;

            depuncturer = new DePuncturer(&param);
        }

        virtual ~DepuncturerTest(){
            delete depuncturer;
            delete [] expectedinputdata_msc_dab;
            delete [] expectedoutputdata_msc_dab;

            delete [] expectedinputdata_msc_dabp;
            delete [] expectedoutputdata_msc_dabp;

            delete [] expectedinputdata_fic;
            delete [] expectedoutputdata_fic;

            delete [] inputdata_msc_dab;
            delete [] outputdata_msc_dab;

            delete [] inputdata_msc_dabp;
            delete [] outputdata_msc_dabp;

            delete [] inputdata_fic;
            delete [] outputdata_fic;
        }

        virtual void SetUp(){
            inputdataLoaded_msc_dab = MatlabIO::ReadData(expectedinputdata_msc_dab, "./data/ut/depuncturer_data_in_msc_11_6_8_7_8_dab.txt", inputlength_msc_dab);
            outputdataLoaded_msc_dab = MatlabIO::ReadData(expectedoutputdata_msc_dab, "./data/ut/depuncturer_data_out_msc_11_6_8_7_8_dab.txt", outputlength_msc_dab);

            inputdataLoaded_msc_dabp = MatlabIO::ReadData(expectedinputdata_msc_dabp, "./data/ut/depuncturer_data_in_msc_8_7_8_dabp.txt", inputlength_msc_dabp);
            outputdataLoaded_msc_dabp = MatlabIO::ReadData(expectedoutputdata_msc_dabp, "./data/ut/depuncturer_data_out_msc_8_7_8_dabp.txt", outputlength_msc_dabp);

            inputdataLoaded_fic = MatlabIO::ReadData(expectedinputdata_fic, "./data/ut/depuncturer_data_in_fic_16_15_8.txt", inputlength_fic);
            outputdataLoaded_fic = MatlabIO::ReadData(expectedoutputdata_fic, "./data/ut/depuncturer_data_out_fic_16_15_8.txt", outputlength_fic);
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

        ModeParameters param;
        DePuncturer * depuncturer;
};


TEST_F(DepuncturerTest, DepuncturerTest_MSC_DAB) {
    ASSERT_TRUE(inputdataLoaded_msc_dab) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_msc_dab) << "TESTING CODE FAILED... could not load output data";

    //inputlength_msc_dab = 24096;
    //outputlength_msc_dab = 43392;

    depuncturer->depunctur_info_.lrange_msc[0] = 836;    depuncturer->depunctur_info_.lpi_msc[0] = 11;
    depuncturer->depunctur_info_.lrange_msc[1] = 1176;   depuncturer->depunctur_info_.lpi_msc[1] = 6;
    depuncturer->depunctur_info_.lrange_msc[2] = 3172;   depuncturer->depunctur_info_.lpi_msc[2] = 8;
    depuncturer->depunctur_info_.lrange_msc[3] = 180;    depuncturer->depunctur_info_.lpi_msc[3] = 7;
    depuncturer->depunctur_info_.lrange_msc[4] = 12;     depuncturer->depunctur_info_.lpi_msc[4] = 8;
    depuncturer->depunctur_info_.padding_msc = 864;
    depuncturer->depunctur_info_.audiolen_msc = 21504;

    depuncturer->depunctur_info_.after_depuncturer_total_len_msc = 0;
    for (size_t i = 0; i < 5; i++)
    	depuncturer->depunctur_info_.after_depuncturer_total_len_msc += ((depuncturer->depunctur_info_.lrange_msc[i]) * 32) / (depuncturer->depunctur_info_.lpi_msc[i]+8);

    for(size_t i =0; i < 4; i++)
    	depuncturer->DePuncturerProcess(expectedinputdata_msc_dab+((21504/4)*i)+(864*i),21504/4,outputdata_msc_dab+(depuncturer->depunctur_info_.after_depuncturer_total_len_msc)*i, true, true);

    for (size_t i = 0; i < outputlength_msc_dab; i++) {
        EXPECT_EQ(outputdata_msc_dab[i], expectedoutputdata_msc_dab[i]) << "vectors differ on " << i;
    }
}

TEST_F(DepuncturerTest, DepuncturerTest_MSC_DABP) {
    ASSERT_TRUE(inputdataLoaded_msc_dabp) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_msc_dabp) << "TESTING CODE FAILED... could not load output data";

    //inputlength_msc_dabp = 23808;
    //outputlength_msc_dabp = 43104;

    depuncturer->depunctur_info_.lrange_msc[0] = 5184;   depuncturer->depunctur_info_.lpi_msc[0] = 8;
    depuncturer->depunctur_info_.lrange_msc[1] = 180;    depuncturer->depunctur_info_.lpi_msc[1] = 7;
    depuncturer->depunctur_info_.lrange_msc[2] = 12;     depuncturer->depunctur_info_.lpi_msc[2] = 8;
    depuncturer->depunctur_info_.lrange_msc[3] = 0;      depuncturer->depunctur_info_.lpi_msc[3] = 0;
    depuncturer->depunctur_info_.lrange_msc[4] = 0;      depuncturer->depunctur_info_.lpi_msc[4] = 0;
    depuncturer->depunctur_info_.padding_msc = 768;
    depuncturer->depunctur_info_.audiolen_msc = 21504;

    depuncturer->depunctur_info_.after_depuncturer_total_len_msc = 0;
    for (size_t i = 0; i < 5; i++)
    	depuncturer->depunctur_info_.after_depuncturer_total_len_msc += ((depuncturer->depunctur_info_.lrange_msc[i]) * 32) / (depuncturer->depunctur_info_.lpi_msc[i]+8);

    for(size_t i =0; i < 4; i++)
    	depuncturer->DePuncturerProcess(expectedinputdata_msc_dabp+((21504/4)*i)+(768*i),21504/4,outputdata_msc_dabp+(depuncturer->depunctur_info_.after_depuncturer_total_len_msc)*i, true, false);

    for (size_t i = 0; i < outputlength_msc_dabp; i++) {
        EXPECT_EQ(outputdata_msc_dabp[i], expectedoutputdata_msc_dabp[i]) << "vectors differ on " << i;
    }
}

TEST_F(DepuncturerTest, DepuncturerTest_FIC) {
    ASSERT_TRUE(inputdataLoaded_fic) << "TESTING CODE FAILED... could not load input data";
    ASSERT_TRUE(outputdataLoaded_fic) << "TESTING CODE FAILED... could not load output data";

    //inputlength_fic = 9216;
    //outputlength_fic = 12384;

    for(size_t i =0; i < 4; i++)
    	depuncturer->DePuncturerProcess(expectedinputdata_fic+((inputlength_fic/4)*i), inputlength_fic/4, outputdata_fic+((outputlength_fic/4)*i), false, false);

    for (size_t i = 0; i < outputlength_fic; i++) {
        EXPECT_EQ(outputdata_fic[i], expectedoutputdata_fic[i]) << "vectors differ on " << i;
    }
}

#endif
