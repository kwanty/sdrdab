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

using namespace std;

class DemodulatorGain42Test  : public ::testing::Test{
    protected:
        DemodulatorGain42Test(){

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


            // Process test

            // mode 1
            process_frame_length = 193952;
            process_sub_frame_length = 64*48*(3 + 4*3);

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

            demodulator = new Demodulator(&mode_1_parameters);
        }

        virtual ~DemodulatorGain42Test(){

            delete demodulator;

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
        }

        virtual void SetUp(){

            // Process test

            // mode 1
            process_input_frame1_loaded = MatlabIO::ReadData(process_input_frame1, "./data/ut/demodulator_process_input_frame1_gain42_long.txt", process_frame_length * 2);
            process_input_frame2_loaded = MatlabIO::ReadData(process_input_frame2, "./data/ut/demodulator_process_input_frame2_gain42_long.txt", process_frame_length * 2);
            process_input_frame3_loaded = MatlabIO::ReadData(process_input_frame3, "./data/ut/demodulator_process_input_frame3_gain42_long.txt", process_frame_length * 2);

            process_frame1_expected_output_loaded = MatlabIO::ReadData(process_frame1_expected_output, "./data/ut/demodulator_process_output_frame1_gain42_long.txt", mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2);
            process_frame2_expected_output_loaded = MatlabIO::ReadData(process_frame2_expected_output, "./data/ut/demodulator_process_output_frame2_gain42_long.txt", mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2);
            process_frame3_expected_output_loaded = MatlabIO::ReadData(process_frame3_expected_output, "./data/ut/demodulator_process_output_frame3_gain42_long.txt", mode_1_parameters.fft_size * 3 / 4 * (mode_1_parameters.number_of_symbols - 1) * 2);

            process_sub_frame1_expected_output_loaded = MatlabIO::ReadData(process_sub_frame1_expected_output, "./data/ut/demodulator_process_sub_output_frame1_gain42_long.txt", process_sub_frame_length);
            process_sub_frame2_expected_output_loaded = MatlabIO::ReadData(process_sub_frame2_expected_output, "./data/ut/demodulator_process_sub_output_frame2_gain42_long.txt", process_sub_frame_length);
            process_sub_frame3_expected_output_loaded = MatlabIO::ReadData(process_sub_frame3_expected_output, "./data/ut/demodulator_process_sub_output_frame3_gain42_long.txt", process_sub_frame_length);
        }

        ModeParameters mode_1_parameters;

        // Process test

        bool process_input_frame1_loaded, process_input_frame2_loaded, process_input_frame3_loaded;
        bool process_frame1_expected_output_loaded, process_frame2_expected_output_loaded, process_frame3_expected_output_loaded;
        bool process_sub_frame1_expected_output_loaded, process_sub_frame2_expected_output_loaded, process_sub_frame3_expected_output_loaded;
        size_t process_frame_length, process_sub_frame_length;

        float *process_input_frame1, *process_input_frame2, *process_input_frame3;
        float *process_frame1_expected_output, *process_frame2_expected_output, *process_frame3_expected_output;
        float *process_sub_frame1_expected_output, *process_sub_frame2_expected_output, *process_sub_frame3_expected_output;

        Demodulator *demodulator;
};


TEST_F(DemodulatorGain42Test, process_frame1_mode_1) {

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

TEST_F(DemodulatorGain42Test, process_frame2_mode_1) {

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

TEST_F(DemodulatorGain42Test, process_frame3_mode_1) {

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

TEST_F(DemodulatorGain42Test, process_sub_frame1_mode_1) {

    ASSERT_TRUE(process_input_frame1_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame1_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame1;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 180;
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

TEST_F(DemodulatorGain42Test, process_sub_frame2_mode_1) {

    ASSERT_TRUE(process_input_frame2_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame2_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame2;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 180;
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

TEST_F(DemodulatorGain42Test, process_sub_frame3_mode_1) {

    ASSERT_TRUE(process_input_frame3_loaded) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(process_sub_frame3_expected_output_loaded) << "TESTING CODE FAILED... could not load expected data";

    float *result = new float[process_sub_frame_length];

    demodReadWrite data_input_output;
    data_input_output.read_here = process_input_frame3;
    data_input_output.write_here = result;

    stationInfo station_info;
    station_info.sub_ch_start_addr = 180;
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
#endif
