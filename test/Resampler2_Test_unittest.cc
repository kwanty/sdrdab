/*
 *
 * @author Grzegorz Skołyszewski skolyszewski.grzegorz@gmail.com (Resampler2)  new 100%
 *
 * @date 12 June 2017 - version 3.0
 * @version 3.0
 *
 * @copyright Copyright (c) 2017 Grzegorz Skołyszewski
 *
 * @par License
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifdef GOOGLE_UNIT_TEST
#include "Resampler/resampler2.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include "math.h"

static bool DifferenceLimit(int val1, int val2, int diff) {
    return !(abs(val1-val2) > diff);
}

using namespace std;

class Resampler2Test : public ::testing::Test{
    protected:
        Resampler2Test() {
                length_ = 8192;
                realistic_ratio_ = 0.99994; // this ratio must match ratio in matlab script used for generating data
                realistic_length_ = size_t(length_/2 * realistic_ratio_) * 2;

                resampler2_ = new Resampler2(Resampler2::LINEAR,length_*4);

                resample_data_loaded_ = true;
                resample_result_loaded_nn_ = true;
                resample_result_loaded_linear_ = true;
                resample_result_loaded_pchip_ = true;
                resample_result_loaded_spline_ = true;
                resample_realistic_result_loaded_nn_ = true;
                resample_realistic_result_loaded_linear_ = true;
                resample_realistic_result_loaded_pchip_ = true;
                resample_realistic_result_loaded_spline_ = true;

                resample_data_ = new float[length_];
                resample_expected_results_nn_ = new float[length_*2];
                resample_expected_results_linear_ = new float[length_*2];
                resample_expected_results_pchip_ = new float[length_*2];
                resample_expected_results_spline_ = new float[length_*2];

                resample_realistic_expected_results_nn_ = new float[length_*2];
                resample_realistic_expected_results_linear_ = new float[length_*2];
                resample_realistic_expected_results_pchip_ = new float[length_*2];
                resample_realistic_expected_results_spline_ = new float[length_*2];
        }

        virtual ~Resampler2Test() {
            delete[] resample_data_;
            delete[] resample_expected_results_nn_;
            delete[] resample_expected_results_linear_;
            delete[] resample_expected_results_pchip_;
            delete[] resample_expected_results_spline_;
            delete[] resample_realistic_expected_results_nn_;
            delete[] resample_realistic_expected_results_linear_;
            delete[] resample_realistic_expected_results_pchip_;
            delete[] resample_realistic_expected_results_spline_;
            delete resampler2_;
        }

        virtual void SetUp() {
            resample_data_loaded_ = MatlabIO::ReadData(resample_data_, "./data/ut/resampler2_data.txt", length_);
            resample_result_loaded_nn_ = MatlabIO::ReadData(resample_expected_results_nn_, "./data/ut/resampler2_nn_expected_results.txt", length_*1.5);
            resample_result_loaded_linear_ = MatlabIO::ReadData(resample_expected_results_linear_, "./data/ut/resampler2_linear_expected_results.txt", length_*1.5);
            resample_result_loaded_pchip_ = MatlabIO::ReadData(resample_expected_results_pchip_, "./data/ut/resampler2_pchip_expected_results.txt", length_*1.5);
            resample_result_loaded_spline_ = MatlabIO::ReadData(resample_expected_results_spline_, "./data/ut/resampler2_spline_expected_results.txt", length_*1.5);

            resample_realistic_result_loaded_nn_ = MatlabIO::ReadData(resample_realistic_expected_results_nn_, "./data/ut/resampler2_realistic_nn_expected_results.txt", realistic_length_);
            resample_realistic_result_loaded_linear_ = MatlabIO::ReadData(resample_realistic_expected_results_linear_, "./data/ut/resampler2_realistic_linear_expected_results.txt", realistic_length_);
            resample_realistic_result_loaded_pchip_ = MatlabIO::ReadData(resample_realistic_expected_results_pchip_, "./data/ut/resampler2_realistic_pchip_expected_results.txt", realistic_length_);
            resample_realistic_result_loaded_spline_ = MatlabIO::ReadData(resample_realistic_expected_results_spline_, "./data/ut/resampler2_realistic_spline_expected_results.txt", realistic_length_);

            resampler2_->debug = 1;
        }

        virtual void TearDown() {
            
        }

        float *resample_data_;
        float *resample_expected_results_nn_;
        float *resample_expected_results_linear_;
        float *resample_expected_results_pchip_;
        float *resample_expected_results_spline_;
        bool resample_data_loaded_;
        bool resample_result_loaded_nn_;
        bool resample_result_loaded_linear_;
        bool resample_result_loaded_pchip_;
        bool resample_result_loaded_spline_;

        float *resample_realistic_expected_results_nn_;
        float *resample_realistic_expected_results_linear_;
        float *resample_realistic_expected_results_pchip_;
        float *resample_realistic_expected_results_spline_;
        bool resample_realistic_result_loaded_nn_;
        bool resample_realistic_result_loaded_linear_;
        bool resample_realistic_result_loaded_pchip_;
        bool resample_realistic_result_loaded_spline_;


        size_t length_, realistic_length_;
        float realistic_ratio_;
        Resampler2 *resampler2_;
};



TEST_F(Resampler2Test, number_of_returned_samples) {
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_nn_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;

    size_t returned_samples, extracted_samples;
    returned_samples = resampler2_->ResampleIntoBuffer(resample_data_,length_,ratio);
    extracted_samples = resampler2_->CopyFromBuffer(result,returned_samples);
    EXPECT_PRED3(DifferenceLimit, length_*ratio, extracted_samples+2,1);
}


TEST_F(Resampler2Test, values_of_returned_nn) {
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_nn_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;
    int fail_counter = 0;
    float limit = 1e-2;

    resampler2_->conv_type_ = 0;
    size_t returned_samples, extracted_samples;
    returned_samples = resampler2_->ResampleIntoBuffer(resample_data_,length_,ratio);
    extracted_samples = resampler2_->CopyFromBuffer(result,returned_samples);

    for(int i=2; i<returned_samples-2; i++) {

        if(float diff = fabs(resample_expected_results_nn_[i]-result[i]) > limit) {
            printf("----------\n");
            for(int j=-9; j<10; j++){
                printf("%2d %f %f\n",i+j,resample_expected_results_nn_[i+j],result[i+j]);
                if(j==0){
                    printf("^\n");
                }
            }
            printf("----------\n");
            fail_counter++;
            /*
            printf("Expected result and calculated result do not match at index %d\n", i);
            printf("%.6f : %.6f \n",resample_expected_results_nn_[i],result[i]);
            printf("Difference: %f\n", diff);
            printf("Failed %d time(s)\n",++fail_counter);
            printf("#####################\n");
            */
        }
        if (fail_counter > 4) {
            FAIL();
        }
    }
}


TEST_F(Resampler2Test, values_of_returned_linear) {
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_linear_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;
    int fail_counter = 0;
    float limit = 1e-1;


    resampler2_->conv_type_ = 1;
    size_t returned_samples, extracted_samples;
    returned_samples = resampler2_->ResampleIntoBuffer(resample_data_,length_,ratio);
    extracted_samples = resampler2_->CopyFromBuffer(result,returned_samples);

    for(int i=0; i<extracted_samples; i++) {
        if(float diff = fabs(resample_expected_results_linear_[i]-result[i]) > limit) {
            printf("Expected result and calculated result do not match at index %d\n", i);
            printf("%.6f : %.6f \n",resample_expected_results_linear_[i],result[i]);
            printf("Difference: %f\n", diff);
            printf("Failed %d time(s)\n",++fail_counter);
            printf("#####################\n");

        }
        if (fail_counter > 4) {
            FAIL();
        }
    }
}

TEST_F(Resampler2Test, continuity_test_nn) {
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_nn_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;
    int fail_counter = 0;
    float limit = 4e-2;

    resampler2_->conv_type_ = 0;

    int block_len = 32;
    size_t returned_samples = 0;

    for(int j = 0; j < 30; j++){
        returned_samples += resampler2_->ResampleIntoBuffer(resample_data_+j*block_len, block_len, ratio);
    }
    resampler2_->CopyFromBuffer(result,returned_samples);

    for(int i=2; i<returned_samples-2; i++) {

        if(float diff = fabs(resample_expected_results_nn_[i]-result[i]) > limit) {
            printf("----------\n");
            for(int j=-9; j<10; j++){
                printf("%2d %f %f\n",i+j,resample_expected_results_nn_[i+j],result[i+j]);
                if(j==0){
                    printf("^\n");
                }
            }
            printf("----------\n");
            fail_counter++;
            /*
            printf("Expected result and calculated result do not match at index %d\n", i);
            printf("%.6f : %.6f \n",resample_expected_results_nn_[i],result[i]);
            printf("Difference: %f\n", diff);
            printf("Failed %d time(s)\n",++fail_counter);
            printf("#####################\n");
            */
        }
        if (fail_counter > 10) {
            FAIL();
        }
    }
}

TEST_F(Resampler2Test, continuity_test_linear) {
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_linear_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;
    int fail_counter = 0;
    float limit = 4e-2;

    resampler2_->conv_type_ = 1;

    int block_len = 32;
    size_t returned_samples = 0;

    for(int j = 0; j < 4; j++){
        returned_samples += resampler2_->ResampleIntoBuffer(resample_data_+j*block_len, block_len, ratio);
    }
    resampler2_->CopyFromBuffer(result,returned_samples);

    for(int i=0; i<returned_samples-0; i++) {
        //printf("%2d %f %f\n",i,resample_expected_results_linear_[i],result[i]);
            
        if(float diff = fabs(resample_expected_results_linear_[i]-result[i]) > limit) {
            printf("----------\n");
            for(int j=-9; j<10; j++){
                printf("%2d %f %f\n",i+j,resample_expected_results_linear_[i+j],result[i+j]);
                if(j==0){
                    printf("^\n");
                }
            }
            printf("----------\n");
            fail_counter++;
            /*
            printf("Expected result and calculated result do not match at index %d\n", i);
            printf("%.6f : %.6f \n",resample_expected_results_linear_[i],result[i]);
            printf("Difference: %f\n", diff);
            printf("Failed %d time(s)\n",++fail_counter);
            printf("#####################\n");
            */
            
        }
        if (fail_counter > 1) {
            FAIL();
        }
    }
}

TEST(Resampler2RatioTest, variable_ratio_test) {
    const int data_length = 10;
    const int ratios_size = 3;

    float ratios[ratios_size] = {0.8, 0.95, 1.2};
    float data[data_length] = {0, 1, 0, -1, 0, 1, 0, -1, 0, 1};
    float result[data_length*2] = {};

    Resampler2 resampler2 = Resampler2(Resampler2::LINEAR,data_length*4);

    size_t returned_samples = 0, extracted_samples = 0;

    for(int i=0; i<ratios_size; i++) {
        returned_samples = resampler2.ResampleIntoBuffer(data,data_length,ratios[i]);
        extracted_samples = resampler2.CopyFromBuffer(result,returned_samples);
        EXPECT_NE(0, extracted_samples) << "process did not return any samples";
    }
}

TEST_F(Resampler2Test, realistic_ratio_test) {
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_realistic_result_loaded_nn_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_realistic_result_loaded_linear_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_realistic_result_loaded_pchip_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_realistic_result_loaded_spline_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    size_t returned_samples = 0, extracted_samples = 0;
    int first_type = 0;
    int last_type = 1;
    int global_fail_counter = 0;

    for(int j = first_type; j <= last_type; j++){
        resampler2_->conv_type_ = j;
        returned_samples = resampler2_->ResampleIntoBuffer(resample_data_,length_,realistic_ratio_);
        extracted_samples = resampler2_->CopyFromBuffer(result,returned_samples);
        EXPECT_PRED3(DifferenceLimit, size_t(length_*realistic_ratio_), extracted_samples,1);

        float * expected_result_pointer;

        switch(j){
            case(0):
                expected_result_pointer = resample_realistic_expected_results_nn_;
                break;
            case(1):
                expected_result_pointer = resample_realistic_expected_results_linear_;
                break;
            case(2):
                expected_result_pointer = resample_realistic_expected_results_pchip_;
                break;
            case(3):
                expected_result_pointer = resample_realistic_expected_results_spline_;
                break;
        }

        int fail_counter = 0;
        float limit = 1e-1;

        for(int i=0; i<returned_samples; i++) {
            if(float diff = fabs(expected_result_pointer[i]-result[i]) > limit) {
                printf("%d %d; %f %f\n",j,i,expected_result_pointer[i],result[i] );
                fail_counter++;

            }
        }
        printf("Fails counted using resampling type %d: %d\n",j,fail_counter);
        global_fail_counter += fail_counter;
    }
    printf("Fails counter for all algorithms: %d\n", global_fail_counter);
    if(global_fail_counter > 2 * (last_type - first_type)){
        FAIL();
    }
}

#endif