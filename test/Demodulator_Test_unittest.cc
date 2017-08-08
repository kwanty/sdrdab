/*
 * demodulator_unittest.cc
 *
 * "Make it count, son" ~ Jerry Reed, 'Amos Moses'
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 */


#ifdef GOOGLE_UNIT_TEST

#include "demodulator.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include "math.h"

class DemodulatorTest  : public ::testing::Test{
    protected:
        DemodulatorTest(){

            mode_1_parameters.frame_size = 196608;
            mode_1_parameters.null_size = 2656;
            mode_1_parameters.fft_size = 2048;
            mode_1_parameters.symbol_size = 2552;
            mode_1_parameters.number_of_carriers = 1536;
            mode_1_parameters.number_of_symbols = 76;
            mode_1_parameters.number_of_symbols_per_fic = 3;
            mode_1_parameters.number_of_cif = 4;
            mode_1_parameters.guard_size = 504;
            mode_1_parameters.number_cu_per_symbol = 48;
            mode_1_parameters.number_symbols_per_cif = 18;

            mode_2_parameters.frame_size = 49152;
            mode_2_parameters.null_size = 664;
            mode_2_parameters.fft_size = 512;
            mode_2_parameters.symbol_size = 638;
            mode_2_parameters.number_of_carriers = 384;
            mode_2_parameters.number_of_symbols = 76;
            mode_2_parameters.number_of_symbols_per_fic = 3;
            mode_2_parameters.number_of_cif = 1;
            mode_2_parameters.guard_size = 126;
            mode_2_parameters.number_cu_per_symbol = 12;
            mode_2_parameters.number_symbols_per_cif = 72;

            mode_3_parameters.fft_size = 256;
            mode_4_parameters.fft_size = 1024;


            // deinterleaver tab generator test

            frequencyDeinterleaverTabMod1Loaded = false;
            frequencyDeinterleaverTabMod2Loaded = false;
            frequencyDeinterleaverTabMod3Loaded = false;
            frequencyDeinterleaverTabMod4Loaded = false;

            expectedFrequencyDeinterleaverTabMod1 = new float[mode_1_parameters.fft_size];
            expectedFrequencyDeinterleaverTabMod2 = new float[mode_2_parameters.fft_size];
            expectedFrequencyDeinterleaverTabMod3 = new float[mode_3_parameters.fft_size];
            expectedFrequencyDeinterleaverTabMod4 = new float[mode_4_parameters.fft_size];

            // deQPSK test
            mem_deqpsk_input_ofdm_symbol = new float[mode_1_parameters.fft_size * mode_1_parameters.number_of_symbols * 2];
            mem_deqpsk_output_ofdm_symbol = new float[mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2];

            deqpsk_input_ofdm_symbol = new float*[mode_1_parameters.number_of_symbols];
            deqpsk_output_ofdm_symbol = new float*[mode_1_parameters.number_of_symbols - 1];
            deqpsk_output_dpsk = new float[mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2];

            deqpsk_input_ofdm_symbol_loaded = false;
            deqpsk_output_dpsk_loaded = false;

            // FramePos & FFTInPlace test
            ofdm_fft_input_data_loaded = false;
            ofdm_output_loaded = false;
            fft_output_loaded = false;

            ofdm_fft_input_data = new float[mode_1_parameters.number_of_symbols * mode_1_parameters.symbol_size * 2];

            mem_ofdm_output = new float[mode_1_parameters.fft_size * mode_1_parameters.number_of_symbols * 2];
            ofdm_output = new float*[mode_1_parameters.number_of_symbols];

            mem_fft_output = new float[mode_1_parameters.fft_size * mode_1_parameters.number_of_symbols * 2];
            fft_output = new float*[mode_1_parameters.number_of_symbols];

            //SNRcalc test
            snr_input_data_loaded = false;
            snr_output_loaded = false;

            snr_input_data = new float[mode_1_parameters.number_of_symbols * mode_1_parameters.symbol_size * 2];
            snr_output = new float[mode_1_parameters.number_of_symbols];

            snr_input_data_mode_2_loaded = false;
            snr_output_mode_2_loaded = false;

            snr_input_data_mode_2 = new float[mode_2_parameters.number_of_symbols * mode_2_parameters.symbol_size * 2];
            snr_output_mode_2 = new float[mode_2_parameters.number_of_symbols];

            // Process test

            // mode 1
            process_frame_length = 193952;
            process_sub_frame_length = 33792;

            process_input_frame1 = new float[process_frame_length * 2];
            process_input_frame2 = new float[process_frame_length * 2];
            process_input_frame3 = new float[process_frame_length * 2];

            process_frame1_expected_output = new float[mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2];
            process_frame2_expected_output = new float[mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2];
            process_frame3_expected_output = new float[mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2];

            process_sub_frame1_expected_output = new float[process_sub_frame_length];
            process_sub_frame2_expected_output = new float[process_sub_frame_length];
            process_sub_frame3_expected_output = new float[process_sub_frame_length];

            process_input_frame1_loaded = false;
            process_input_frame2_loaded = false;
            process_input_frame3_loaded = false;

            process_frame1_expected_output_loaded = false;
            process_frame2_expected_output_loaded = false;
            process_frame3_expected_output_loaded = false;

            process_sub_frame1_expected_output_loaded = false;
            process_sub_frame2_expected_output_loaded = false;
            process_sub_frame3_expected_output_loaded = false;

            // mode 2
            process_frame_length_mode_2 = 48488;
            process_sub_frame_length_mode_2 = 7680;

            process_input_frame1_mode_2 = new float[process_frame_length_mode_2 * 2];
            process_input_frame2_mode_2 = new float[process_frame_length_mode_2 * 2];
            process_input_frame3_mode_2 = new float[process_frame_length_mode_2 * 2];

            process_frame1_expected_output_mode_2 = new float[mode_2_parameters.fft_size * 3 / 4 * (mode_2_parameters.number_of_symbols - 1) * 2];
            process_frame2_expected_output_mode_2 = new float[mode_2_parameters.fft_size * 3 / 4 * (mode_2_parameters.number_of_symbols - 1) * 2];
            process_frame3_expected_output_mode_2 = new float[mode_2_parameters.fft_size * 3 / 4 * (mode_2_parameters.number_of_symbols - 1) * 2];

            process_sub_frame1_expected_output_mode_2 = new float[process_sub_frame_length_mode_2];
            process_sub_frame2_expected_output_mode_2 = new float[process_sub_frame_length_mode_2];
            process_sub_frame3_expected_output_mode_2 = new float[process_sub_frame_length_mode_2];

            process_input_frame1_loaded_mode_2 = false;
            process_input_frame2_loaded_mode_2 = false;
            process_input_frame3_loaded_mode_2 = false;

            process_frame1_expected_output_loaded_mode_2 = false;
            process_frame2_expected_output_loaded_mode_2 = false;
            process_frame3_expected_output_loaded_mode_2 = false;

            process_sub_frame1_expected_output_loaded_mode_2 = false;
            process_sub_frame2_expected_output_loaded_mode_2 = false;
            process_sub_frame3_expected_output_loaded_mode_2 = false;

            demodulator = new Demodulator(&mode_1_parameters);
            demodulator_mode_2 = new Demodulator(&mode_2_parameters);
            demodulator_for_snr = new Demodulator(&mode_1_parameters);
            demodulator_for_snr_mode_2 = new Demodulator(&mode_2_parameters);
        }

        virtual ~DemodulatorTest(){

            delete demodulator;
            delete demodulator_mode_2;
            delete demodulator_for_snr;
      	    delete demodulator_for_snr_mode_2;

            delete [] expectedFrequencyDeinterleaverTabMod1;
            delete [] expectedFrequencyDeinterleaverTabMod2;
            delete [] expectedFrequencyDeinterleaverTabMod3;
            delete [] expectedFrequencyDeinterleaverTabMod4;


            delete [] frequencyDeinterleaverTabMod1;
            delete [] frequencyDeinterleaverTabMod2;
            delete [] frequencyDeinterleaverTabMod3;
            delete [] frequencyDeinterleaverTabMod4;

            delete [] mem_deqpsk_input_ofdm_symbol;
            delete [] mem_deqpsk_output_ofdm_symbol;
            delete [] deqpsk_output_dpsk;
            //delete [] deqpsk_input_ofdm_symbol; // already deallocated in Demodulator
            delete [] deqpsk_output_ofdm_symbol;

            delete [] ofdm_fft_input_data;
            //delete [] ofdm_output; // already deallocated in Demodulator
            delete [] mem_ofdm_output;
            delete [] fft_output;
            delete [] mem_fft_output;

            delete [] snr_input_data;
      	    delete [] snr_output;
      	    delete [] snr_input_data_mode_2;
      	    delete [] snr_output_mode_2;

            // mode 1
            delete [] process_input_frame1;
            delete [] process_input_frame2;
            delete [] process_input_frame3;

            delete [] process_frame1_expected_output;
            delete [] process_frame2_expected_output;
            delete [] process_frame3_expected_output;

            delete [] process_sub_frame1_expected_output;
            delete [] process_sub_frame2_expected_output;
            delete [] process_sub_frame3_expected_output;

            // mode 2
            delete [] process_input_frame1_mode_2;
            delete [] process_input_frame2_mode_2;
            delete [] process_input_frame3_mode_2;

            delete [] process_frame1_expected_output_mode_2;
            delete [] process_frame2_expected_output_mode_2;
            delete [] process_frame3_expected_output_mode_2;

            delete [] process_sub_frame1_expected_output_mode_2;
            delete [] process_sub_frame2_expected_output_mode_2;
            delete [] process_sub_frame3_expected_output_mode_2;
        }

        virtual void SetUp(){

            // deinterleaver tab gen test

            frequencyDeinterleaverTabMod1Loaded = MatlabIO::ReadData(expectedFrequencyDeinterleaverTabMod1, "./data/ut/demodulator_deinterleaver_tab_mode_1.txt", mode_1_parameters.fft_size*3/4);
            frequencyDeinterleaverTabMod1 = demodulator->GenerateFrequencyDeInterleaverTab(mode_1_parameters.fft_size);

            frequencyDeinterleaverTabMod2Loaded = MatlabIO::ReadData(expectedFrequencyDeinterleaverTabMod2, "./data/ut/demodulator_deinterleaver_tab_mode_2.txt", mode_2_parameters.fft_size*3/4);
            frequencyDeinterleaverTabMod2 = demodulator->GenerateFrequencyDeInterleaverTab(mode_2_parameters.fft_size);

            frequencyDeinterleaverTabMod3Loaded = MatlabIO::ReadData(expectedFrequencyDeinterleaverTabMod3, "./data/ut/demodulator_deinterleaver_tab_mode_3.txt", mode_3_parameters.fft_size*3/4);
            frequencyDeinterleaverTabMod3 = demodulator->GenerateFrequencyDeInterleaverTab(mode_3_parameters.fft_size);

            frequencyDeinterleaverTabMod4Loaded = MatlabIO::ReadData(expectedFrequencyDeinterleaverTabMod4, "./data/ut/demodulator_deinterleaver_tab_mode_4.txt", mode_4_parameters.fft_size*3/4);
            frequencyDeinterleaverTabMod4 = demodulator->GenerateFrequencyDeInterleaverTab(mode_4_parameters.fft_size);

            // deQPSK test
            deqpsk_input_ofdm_symbol_loaded = MatlabIO::ReadData(mem_deqpsk_input_ofdm_symbol, "./data/ut/demodulator_deqpsk_input_mode_1.txt", mode_1_parameters.fft_size * mode_1_parameters.number_of_symbols * 2);
            for (size_t i = 0; i < mode_1_parameters.number_of_symbols; i++) {
                deqpsk_input_ofdm_symbol[i] = mem_deqpsk_input_ofdm_symbol + i * mode_1_parameters.fft_size * 2;
            }

            deqpsk_output_dpsk_loaded = MatlabIO::ReadData(deqpsk_output_dpsk, "./data/ut/demodulator_deqpsk_output_dpsk_mode_1.txt", mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2);

            // FramePos & FFTInPlace test
            ofdm_fft_input_data_loaded = MatlabIO::ReadData(ofdm_fft_input_data, "./data/ut/demodulator_ofdm_fft_input_mode_1.txt", mode_1_parameters.number_of_symbols * mode_1_parameters.symbol_size * 2);
            ofdm_output_loaded = MatlabIO::ReadData(mem_ofdm_output, "./data/ut/demodulator_ofdm_output_mode_1.txt", mode_1_parameters.fft_size * mode_1_parameters.number_of_symbols * 2);
            for (size_t i = 0; i < mode_1_parameters.number_of_symbols; i++) {
                ofdm_output[i] = mem_ofdm_output + i * mode_1_parameters.fft_size * 2;
            }

            fft_output_loaded = MatlabIO::ReadData(mem_fft_output, "./data/ut/demodulator_fft_output_mode_1.txt", mode_1_parameters.fft_size * mode_1_parameters.number_of_symbols * 2);
            for (size_t i = 0; i < mode_1_parameters.number_of_symbols; i++) {
                fft_output[i] = mem_fft_output + i * mode_1_parameters.fft_size * 2;
            }

            // SNRcalc test
            snr_output_loaded = MatlabIO::ReadData(snr_output, "./data/ut/demodulator_snr_output.txt", mode_1_parameters.number_of_symbols);

            snr_input_data_loaded = MatlabIO::ReadData(snr_input_data, "./data/ut/demodulator_snr_input.txt", mode_1_parameters.symbol_size * mode_1_parameters.number_of_symbols*2);

            snr_output_mode_2_loaded = MatlabIO::ReadData(snr_output_mode_2, "./data/ut/demodulator_snr_output_mode_2.txt", mode_2_parameters.number_of_symbols);

            snr_input_data_mode_2_loaded = MatlabIO::ReadData(snr_input_data_mode_2, "./data/ut/demodulator_snr_input_mode_2.txt", mode_2_parameters.symbol_size * mode_2_parameters.number_of_symbols*2);


            // Process test
            // mode 1
            process_input_frame1_loaded = MatlabIO::ReadData(process_input_frame1, "./data/ut/demodulator_process_input_frame1_mode_1.txt", process_frame_length * 2);
            process_input_frame2_loaded = MatlabIO::ReadData(process_input_frame2, "./data/ut/demodulator_process_input_frame2_mode_1.txt", process_frame_length * 2);
            process_input_frame3_loaded = MatlabIO::ReadData(process_input_frame3, "./data/ut/demodulator_process_input_frame3_mode_1.txt", process_frame_length * 2);

            process_frame1_expected_output_loaded = MatlabIO::ReadData(process_frame1_expected_output, "./data/ut/demodulator_process_output_frame1_mode_1.txt", mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2);
            process_frame2_expected_output_loaded = MatlabIO::ReadData(process_frame2_expected_output, "./data/ut/demodulator_process_output_frame2_mode_1.txt", mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2);
            process_frame3_expected_output_loaded = MatlabIO::ReadData(process_frame3_expected_output, "./data/ut/demodulator_process_output_frame3_mode_1.txt", mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2);

            process_sub_frame1_expected_output_loaded = MatlabIO::ReadData(process_sub_frame1_expected_output, "./data/ut/demodulator_process_sub_output_frame1_mode_1.txt", process_sub_frame_length);
            process_sub_frame2_expected_output_loaded = MatlabIO::ReadData(process_sub_frame2_expected_output, "./data/ut/demodulator_process_sub_output_frame2_mode_1.txt", process_sub_frame_length);
            process_sub_frame3_expected_output_loaded = MatlabIO::ReadData(process_sub_frame3_expected_output, "./data/ut/demodulator_process_sub_output_frame3_mode_1.txt", process_sub_frame_length);

            // mode 2
            process_input_frame1_loaded_mode_2 = MatlabIO::ReadData(process_input_frame1_mode_2, "./data/ut/demodulator_process_input_frame1_mode_2.txt", process_frame_length_mode_2 * 2);
            process_input_frame2_loaded_mode_2 = MatlabIO::ReadData(process_input_frame2_mode_2, "./data/ut/demodulator_process_input_frame2_mode_2.txt", process_frame_length_mode_2 * 2);
            process_input_frame3_loaded_mode_2 = MatlabIO::ReadData(process_input_frame3_mode_2, "./data/ut/demodulator_process_input_frame3_mode_2.txt", process_frame_length_mode_2 * 2);

            process_frame1_expected_output_loaded_mode_2 = MatlabIO::ReadData(process_frame1_expected_output_mode_2, "./data/ut/demodulator_process_output_frame1_mode_2.txt", mode_2_parameters.fft_size * 3 / 4 * (mode_2_parameters.number_of_symbols - 1) * 2);
            process_frame2_expected_output_loaded_mode_2 = MatlabIO::ReadData(process_frame2_expected_output_mode_2, "./data/ut/demodulator_process_output_frame2_mode_2.txt", mode_2_parameters.fft_size * 3 / 4 * (mode_2_parameters.number_of_symbols - 1) * 2);
            process_frame3_expected_output_loaded_mode_2 = MatlabIO::ReadData(process_frame3_expected_output_mode_2, "./data/ut/demodulator_process_output_frame3_mode_2.txt", mode_2_parameters.fft_size * 3 / 4 * (mode_2_parameters.number_of_symbols - 1) * 2);

            process_sub_frame1_expected_output_loaded_mode_2 = MatlabIO::ReadData(process_sub_frame1_expected_output_mode_2, "./data/ut/demodulator_process_sub_output_frame1_mode_2.txt", process_sub_frame_length_mode_2);
            process_sub_frame2_expected_output_loaded_mode_2 = MatlabIO::ReadData(process_sub_frame2_expected_output_mode_2, "./data/ut/demodulator_process_sub_output_frame2_mode_2.txt", process_sub_frame_length_mode_2);
            process_sub_frame3_expected_output_loaded_mode_2 = MatlabIO::ReadData(process_sub_frame3_expected_output_mode_2, "./data/ut/demodulator_process_sub_output_frame3_mode_2.txt", process_sub_frame_length_mode_2);
        }

        ModeParameters mode_1_parameters;
        ModeParameters mode_2_parameters;
        ModeParameters mode_3_parameters;
        ModeParameters mode_4_parameters;

        // de-interleaver tab test

        float *expectedFrequencyDeinterleaverTabMod1, *expectedFrequencyDeinterleaverTabMod2, *expectedFrequencyDeinterleaverTabMod3, *expectedFrequencyDeinterleaverTabMod4;
        int *frequencyDeinterleaverTabMod1, *frequencyDeinterleaverTabMod2, *frequencyDeinterleaverTabMod3, *frequencyDeinterleaverTabMod4;
        bool frequencyDeinterleaverTabMod1Loaded, frequencyDeinterleaverTabMod2Loaded, frequencyDeinterleaverTabMod3Loaded, frequencyDeinterleaverTabMod4Loaded;

        // deQPSK test

        bool deqpsk_input_ofdm_symbol_loaded, deqpsk_output_dpsk_loaded;
        float **deqpsk_input_ofdm_symbol;
        float **deqpsk_output_ofdm_symbol;
        float *deqpsk_output_dpsk;
        float *mem_deqpsk_input_ofdm_symbol, *mem_deqpsk_output_ofdm_symbol;

        // FramePos & FFTInPlace test

        bool ofdm_fft_input_data_loaded, ofdm_output_loaded, fft_output_loaded;
        float *ofdm_fft_input_data;
        float **ofdm_output;
        float *mem_ofdm_output;
        float **fft_output;
        float *mem_fft_output;

        // SNRcalc test
      	bool snr_output_loaded, snr_input_data_loaded;
      	float *snr_input_data;
      	float *snr_output;

      	bool snr_output_mode_2_loaded, snr_input_data_mode_2_loaded;
      	float *snr_input_data_mode_2;
      	float *snr_output_mode_2;

        // Process test

        bool process_input_frame1_loaded, process_input_frame2_loaded, process_input_frame3_loaded;
        bool process_frame1_expected_output_loaded, process_frame2_expected_output_loaded, process_frame3_expected_output_loaded;
        bool process_sub_frame1_expected_output_loaded, process_sub_frame2_expected_output_loaded, process_sub_frame3_expected_output_loaded;
        size_t process_frame_length, process_sub_frame_length;

        float *process_input_frame1, *process_input_frame2, *process_input_frame3;
        float *process_frame1_expected_output, *process_frame2_expected_output, *process_frame3_expected_output;
        float *process_sub_frame1_expected_output, *process_sub_frame2_expected_output, *process_sub_frame3_expected_output;

        // Proces - mode 2
        bool process_input_frame1_loaded_mode_2, process_input_frame2_loaded_mode_2, process_input_frame3_loaded_mode_2;
        bool process_frame1_expected_output_loaded_mode_2, process_frame2_expected_output_loaded_mode_2, process_frame3_expected_output_loaded_mode_2;
        bool process_sub_frame1_expected_output_loaded_mode_2, process_sub_frame2_expected_output_loaded_mode_2, process_sub_frame3_expected_output_loaded_mode_2;
        size_t process_frame_length_mode_2, process_sub_frame_length_mode_2;

        float *process_input_frame1_mode_2, *process_input_frame2_mode_2, *process_input_frame3_mode_2;
        float *process_frame1_expected_output_mode_2, *process_frame2_expected_output_mode_2, *process_frame3_expected_output_mode_2;
        float *process_sub_frame1_expected_output_mode_2, *process_sub_frame2_expected_output_mode_2, *process_sub_frame3_expected_output_mode_2;

        // -----------

        Demodulator *demodulator, *demodulator_mode_2, *demodulator_for_snr, *demodulator_for_snr_mode_2;
};


TEST_F(DemodulatorTest, deinterleaver_tab_mode_1) {

    ASSERT_TRUE(frequencyDeinterleaverTabMod1Loaded) << "TESTING CODE FAILED... could not load expected data";

    float *expectedResult = expectedFrequencyDeinterleaverTabMod1;
    int fail_counter = 0;

    for (size_t i = 0; i < mode_1_parameters.fft_size * 3 / 4; i++) {
        EXPECT_EQ(frequencyDeinterleaverTabMod1[i], expectedResult[i]) << "vectors differ "<< frequencyDeinterleaverTabMod1[i] << " vs " << expectedResult[i] << " at indexes " << i;
        if(frequencyDeinterleaverTabMod1[i] != expectedResult[i]) {
            fail_counter++;
        }

        if(fail_counter >= 10) {
            FAIL() << "TESTING ABORTED due to massive vector difference";
        }
    }
}

TEST_F(DemodulatorTest, deinterleaver_tab_mode_2) {
    ASSERT_TRUE(frequencyDeinterleaverTabMod2Loaded) << "TESTING CODE FAILED... could not load expected data";

    float *expectedResult = expectedFrequencyDeinterleaverTabMod2;
    int fail_counter = 0;

    for (size_t i = 0; i < mode_2_parameters.fft_size * 3 / 4; i++) {
        EXPECT_EQ(frequencyDeinterleaverTabMod2[i], expectedResult[i]) << "vectors differ "<< frequencyDeinterleaverTabMod2[i] << " vs " << expectedResult[i] << " at indexes " << i;
        if(frequencyDeinterleaverTabMod2[i] != expectedResult[i]) {
            fail_counter++;
        }

        if(fail_counter >= 10) {
            FAIL() << "TESTING ABORTED due to massive vector difference";
        }
    }
}

TEST_F(DemodulatorTest, deinterleaver_tab_mode_3) {
    ASSERT_TRUE(frequencyDeinterleaverTabMod3Loaded) << "TESTING CODE FAILED... could not load expected data";

    float *expectedResult = expectedFrequencyDeinterleaverTabMod3;
    int fail_counter = 0;

    for (size_t i = 0; i < mode_3_parameters.fft_size * 3 / 4; i++) {
        EXPECT_EQ(frequencyDeinterleaverTabMod3[i], expectedResult[i]) << "vectors differ "<< frequencyDeinterleaverTabMod3[i] << " vs " << *expectedResult << " at indexes " << i;
        if(frequencyDeinterleaverTabMod3[i] != expectedResult[i]) {
            fail_counter++;
        }

        if(fail_counter >= 10) {
            FAIL() << "TESTING ABORTED due to massive vector difference";
        }
    }
}

TEST_F(DemodulatorTest, deinterleaver_tab_mode_4) {
    ASSERT_TRUE(frequencyDeinterleaverTabMod4Loaded) << "TESTING CODE FAILED... could not load expected data";

    float *expectedResult = expectedFrequencyDeinterleaverTabMod4;
    int fail_counter = 0;

    for (size_t i = 0; i < mode_4_parameters.fft_size * 3 / 4; i++) {
        EXPECT_EQ(frequencyDeinterleaverTabMod4[i], expectedResult[i]) << "vectors differ "<< frequencyDeinterleaverTabMod4[i] << " vs " << expectedResult[i] << " at indexes " << i;
        if(frequencyDeinterleaverTabMod4[i] != expectedResult[i]) {
            fail_counter++;
        }

        if(fail_counter >= 10) {
            FAIL() << "TESTING ABORTED due to massive vector difference";
        }
    }
}

TEST_F(DemodulatorTest, deqpsk_mode_1) {

    ASSERT_TRUE(deqpsk_input_ofdm_symbol_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(deqpsk_output_dpsk_loaded) << "TESTING CODE FAILED... could not load expected data";

    size_t StartAddress = 0;
    size_t SubChannelSize = 864;

    demodulator->ofdm_symbol_ = deqpsk_input_ofdm_symbol;
    demodulator->symb_start_addr_ = floor(StartAddress / mode_1_parameters.number_cu_per_symbol);
    demodulator->symb_end_addr_ = floor((StartAddress + SubChannelSize - 1) / mode_1_parameters.number_cu_per_symbol);

    float *demodulator_output = new float[(mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2];

    demodulator->DeQPSK(demodulator_output);

    for (size_t i = 0; i < (mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2; i++) {
        EXPECT_EQ(deqpsk_output_dpsk[i], demodulator_output[i]) << "vectors differ at indexes " << i;
        if (demodulator_output[i] != deqpsk_output_dpsk[i]) {
            FAIL() << "TESTING ABORTED due to single vector difference in deqpsk_output_dpsk";
        }
    }

    delete [] demodulator_output;
}

TEST_F(DemodulatorTest, calc_frame_pos_mode_1) {

    ASSERT_TRUE(ofdm_fft_input_data_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(ofdm_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    demodulator->CalculateFramePosition(ofdm_fft_input_data);

    for (size_t i = 0; i < mode_1_parameters.number_of_symbols; i++) {
        for (size_t j  = 0; j < mode_1_parameters.fft_size * 2; j++) {
            EXPECT_EQ(ofdm_output[i][j], demodulator->ofdm_symbol_[i][j]) << "vectors differ at indexes " << i << ", " << j;
            if (ofdm_output[i][j] != demodulator->ofdm_symbol_[i][j]) {
                FAIL() << "TESTING ABORTED due to single vector difference";
            }
        }
    }
}

TEST_F(DemodulatorTest, fft_in_place_mode_1) {

    ASSERT_TRUE(ofdm_output_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(fft_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    demodulator->ofdm_symbol_ = ofdm_output;
    demodulator->symb_start_addr_ = 0;
    demodulator->symb_end_addr_ = 17;

    demodulator->FFTInPlace(mode_1_parameters.fft_size);

    int fail_counter = 0;

    for (size_t i = 0; i < mode_1_parameters.number_of_symbols; i++) {
        for (size_t j  = 0; j < mode_1_parameters.fft_size * 2; j++) {
            EXPECT_NEAR(fft_output[i][j], demodulator->ofdm_symbol_[i][j], 1e-2) << "vectors differ at indexes " << i << ", " << j << " (" << ++fail_counter << " fails)";
            if (fail_counter > 0) {
                FAIL() << "TESTING ABORTED due to single vector difference";
            }
        }
    }
}

TEST_F(DemodulatorTest, SNR) {

    ASSERT_TRUE(snr_output_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(snr_input_data_loaded) << "TESTING CODE FAILED... could not load expected data";

    demodulator_for_snr->CalculateFramePosition(snr_input_data);
    demodulator_for_snr->SNRcalc(demodulator_for_snr->output_snr_);

    int fail_counter = 0;

    for (size_t i = 0; i < mode_1_parameters.number_of_symbols; i++) {
        EXPECT_NEAR(snr_output[i], demodulator_for_snr->output_snr_[i], 1e-4) << "vectors differ at index " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }
}

TEST_F(DemodulatorTest, SNR_mode_2) {

    ASSERT_TRUE(snr_output_mode_2_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(snr_input_data_mode_2_loaded) << "TESTING CODE FAILED... could not load expected data";

    demodulator_for_snr_mode_2->CalculateFramePosition(snr_input_data_mode_2);
    demodulator_for_snr_mode_2->SNRcalc(demodulator_for_snr_mode_2->output_snr_);

    int fail_counter = 0;

    for (size_t i = 0; i < mode_2_parameters.number_of_symbols; i++) {
        EXPECT_NEAR(snr_output_mode_2[i], demodulator_for_snr_mode_2->output_snr_[i], 1e-4) << "vectors differ at index " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }
}

TEST_F(DemodulatorTest, process_frame1_mode_1) {

    ASSERT_TRUE(process_input_frame1_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_frame1_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[(mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame1;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 864;

    demodulator->Process(&station_info, &data_input_output);

    int fail_counter = 0;

    for (size_t i = 0; i < (mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2; i++) {
        EXPECT_NEAR(process_frame1_expected_output[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_frame2_mode_1) {

    ASSERT_TRUE(process_input_frame2_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_frame2_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[(mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame2;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 864;

    demodulator->Process(&station_info, &data_input_output);

    int fail_counter = 0;

    for (size_t i = 0; i < (mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2; i++) {
        EXPECT_NEAR(process_frame2_expected_output[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_frame3_mode_1) {

    ASSERT_TRUE(process_input_frame3_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_frame3_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[(mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame3;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 864;

    demodulator->Process(&station_info, &data_input_output);

    int fail_counter = 0;

    for (size_t i = 0; i < (mode_1_parameters.number_of_symbols - 1) * mode_1_parameters.fft_size * 3 / 4 * 2; i++) {
        EXPECT_NEAR(process_frame3_expected_output[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_sub_frame1_mode_1) {

    ASSERT_TRUE(process_input_frame1_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame1_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame1;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 84;

    demodulator->Process(&station_info, &data_input_output);

    int fail_counter = 0;

    for (size_t i = 0; i < process_sub_frame_length; i++) {
        EXPECT_NEAR(process_sub_frame1_expected_output[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_sub_frame2_mode_1) {

    ASSERT_TRUE(process_input_frame2_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame2_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame2;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 84;

    demodulator->Process(&station_info, &data_input_output);

    int fail_counter = 0;

    for (size_t i = 0; i < process_sub_frame_length; i++) {
        EXPECT_NEAR(process_sub_frame2_expected_output[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_sub_frame3_mode_1) {

    ASSERT_TRUE(process_input_frame3_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame3_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame3;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 84;

    demodulator->Process(&station_info, &data_input_output);

    int fail_counter = 0;

    for (size_t i = 0; i < process_sub_frame_length; i++) {
        EXPECT_NEAR(process_sub_frame3_expected_output[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_frame1_mode_2) {

    ASSERT_TRUE(process_input_frame1_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_frame1_expected_output_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[(mode_2_parameters.number_of_symbols - 1) * mode_2_parameters.fft_size * 3 / 4 * 2];

    demodReadWrite data_input_output_mode_2;
    data_input_output_mode_2.read_here = process_input_frame1_mode_2;
    data_input_output_mode_2.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 864;

    demodulator_mode_2->Process(&station_info, &data_input_output_mode_2);

    int fail_counter = 0;

    for (size_t i = 0; i < (mode_2_parameters.number_of_symbols - 1) * mode_2_parameters.fft_size * 3 / 4 * 2; i++) {
        EXPECT_NEAR(process_frame1_expected_output_mode_2[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_frame2_mode_2) {

    ASSERT_TRUE(process_input_frame2_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_frame2_expected_output_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[(mode_2_parameters.number_of_symbols - 1) * mode_2_parameters.fft_size * 3 / 4 * 2];

    demodReadWrite data_input_output_mode_2;
    data_input_output_mode_2.read_here = process_input_frame2_mode_2;
    data_input_output_mode_2.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 864;

    demodulator_mode_2->Process(&station_info, &data_input_output_mode_2);

    int fail_counter = 0;

    for (size_t i = 0; i < (mode_2_parameters.number_of_symbols - 1) * mode_2_parameters.fft_size * 3 / 4 * 2; i++) {
        EXPECT_NEAR(process_frame2_expected_output_mode_2[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_frame3_mode_2) {

    ASSERT_TRUE(process_input_frame3_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_frame3_expected_output_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[(mode_2_parameters.number_of_symbols - 1) * mode_2_parameters.fft_size * 3 / 4 * 2];

    demodReadWrite data_input_output_mode_2;
    data_input_output_mode_2.read_here = process_input_frame3_mode_2;
    data_input_output_mode_2.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 864;

    demodulator_mode_2->Process(&station_info, &data_input_output_mode_2);

    int fail_counter = 0;

    for (size_t i = 0; i < (mode_2_parameters.number_of_symbols - 1) * mode_2_parameters.fft_size * 3 / 4 * 2; i++) {
        EXPECT_NEAR(process_frame3_expected_output_mode_2[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_sub_frame1_mode_2) {

    ASSERT_TRUE(process_input_frame1_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame1_expected_output_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length_mode_2];

    demodReadWrite data_input_output_mode_2;
    data_input_output_mode_2.read_here = process_input_frame1_mode_2;
    data_input_output_mode_2.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 84;

    demodulator_mode_2->Process(&station_info, &data_input_output_mode_2);

    int fail_counter = 0;

    for (size_t i = 0; i < process_sub_frame_length_mode_2; i++) {
        EXPECT_NEAR(process_sub_frame1_expected_output_mode_2[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_sub_frame2_mode_2) {

    ASSERT_TRUE(process_input_frame2_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame2_expected_output_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length_mode_2];

    demodReadWrite data_input_output_mode_2;
    data_input_output_mode_2.read_here = process_input_frame2_mode_2;
    data_input_output_mode_2.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 84;

    demodulator_mode_2->Process(&station_info, &data_input_output_mode_2);

    int fail_counter = 0;

    for (size_t i = 0; i < process_sub_frame_length_mode_2; i++) {
        EXPECT_NEAR(process_sub_frame2_expected_output_mode_2[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}

TEST_F(DemodulatorTest, process_sub_frame3_mode_2) {

    ASSERT_TRUE(process_input_frame3_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame3_expected_output_loaded_mode_2) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length_mode_2];

    demodReadWrite data_input_output_mode_2;
    data_input_output_mode_2.read_here = process_input_frame3_mode_2;
    data_input_output_mode_2.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 0;
    station_info.sub_ch_size = 84;

    demodulator_mode_2->Process(&station_info, &data_input_output_mode_2);

    int fail_counter = 0;

    for (size_t i = 0; i < process_sub_frame_length_mode_2; i++) {
        EXPECT_NEAR(process_sub_frame3_expected_output_mode_2[i], result[i], 1e-3) << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)";
        if (fail_counter > 0) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }

    delete [] result;
}
#endif
