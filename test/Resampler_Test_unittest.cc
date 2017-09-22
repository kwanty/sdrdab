#include "Resampler/resampler.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include "math.h"

static bool DifferenceLimit(int val1, int val2, int diff) {
    return !(abs(val1-val2) > diff);
}

class ResamplerTestLinear: public testing::Test {
    protected:

        ResamplerTestLinear():
            resampler_(NULL) {
                length_ = 4096;
                channels_ = 1;
                conv_type_ = SRC_SINC_MEDIUM_QUALITY;

                resample_data_loaded_ = false;
                resample_result_loaded_ = false;

                resample_data_ = new float[length_];
                resample_expected_results_ = new float[length_*2];
            }

        virtual ~ResamplerTestLinear() {
            delete[] resample_data_;
            delete[] resample_expected_results_;
        }

        virtual void SetUp() {
            resample_data_loaded_ = MatlabIO::ReadData(resample_data_, "./data/ut/resampler_linear_data.txt", length_);
            resample_result_loaded_ = MatlabIO::ReadData(resample_expected_results_, "./data/ut/resampler_linear_expected_results.txt", length_*1.5);

            resampler_ = new Resampler(conv_type_, channels_);
        }

        virtual void TearDown() {
            delete resampler_;
        }

        float *resample_data_;
        float *resample_expected_results_;
        bool resample_data_loaded_;
        bool resample_result_loaded_;

        size_t length_;
        int channels_;
        int conv_type_;
        Resampler *resampler_;
};

class ResamplerTestInterleaved: public testing::Test {
    protected:

        ResamplerTestInterleaved():
            resampler_(NULL) {
                length_ = 8192;
                channels_ = 2;
                conv_type_ = SRC_SINC_MEDIUM_QUALITY;

                resample_data_loaded_ = false;
                resample_result_loaded_ = false;

                resample_data_ = new float[length_];
                resample_expected_results_ = new float[length_*2];

                resampler_ = new Resampler(conv_type_, channels_);
            }

        virtual ~ResamplerTestInterleaved() {
            delete[] resample_data_;
            delete[] resample_expected_results_;
        }

        virtual void SetUp() {
            resample_data_loaded_ = MatlabIO::ReadData(resample_data_, "./data/ut/resampler_interleaved_data.txt", length_);
            resample_result_loaded_ = MatlabIO::ReadData(resample_expected_results_, "./data/ut/resampler_interleaved_expected_results.txt", length_*1.5);

           
        }

        virtual void TearDown() {
            delete resampler_;
        }

        float *resample_data_;
        float *resample_expected_results_;
        bool resample_data_loaded_;
        bool resample_result_loaded_;

        size_t length_;
        int channels_;
        int conv_type_;
        Resampler *resampler_;
};

TEST_F(ResamplerTestLinear, number_of_returned_samples) {

    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;

    resampler_->SetSourceBuffer(resample_data_, length_);
    int returned_samples = resampler_->Resample(result, length_*2, ratio);

    EXPECT_PRED3(DifferenceLimit, length_*ratio, returned_samples, channels_);
}

TEST_F(ResamplerTestLinear, values_of_returned) {

    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;
    int fail_counter = 0;
    float limit = 3e-2;

    resampler_->SetSourceBuffer(resample_data_, length_);
    int returned_samples = resampler_->Resample(result, length_*2, ratio);

    for(int i=0; i<returned_samples; i++) {
        if(int diff = fabs(resample_expected_results_[i]-result[i]) > limit) {
            cout << "The difference between resample_expected_results_[i] and result[i] is " << diff
                << ", which exceeds limit"
                << ", where resample_expected_results_[i] evaluates to " << resample_expected_results_[i]
                << ", result[i] evaluates to " << result[i]
                << ", and limit evaluates to " << limit << endl
                << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)" << endl
                << "vectors can differ at max " << channels_ << " times" << endl;
        }

        if (fail_counter > channels_) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }
}

TEST_F(ResamplerTestLinear, short_buffer_test) {

    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*1];
    float ratio = 1.5;
    size_t ex_len = length_*ratio;

    resampler_->SetSourceBuffer(resample_data_, length_);
    int returned_samples = resampler_->Resample(result, length_, ratio);

    cout << "Number of returned samples at first " << returned_samples << endl;

    EXPECT_PRED3(DifferenceLimit, length_, returned_samples, channels_);

    returned_samples = resampler_->Resample(result, length_, ratio);

    cout << "Number of returned samples at second " << returned_samples << endl;

    EXPECT_PRED3(DifferenceLimit, ex_len-length_, returned_samples, channels_);
}

TEST_F(ResamplerTestInterleaved, number_of_returned_samples) {

    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;

    resampler_->SetSourceBuffer(resample_data_, length_);
    int returned_samples = resampler_->Resample(result, length_*2, ratio);

    EXPECT_PRED3(DifferenceLimit, length_*ratio, returned_samples, channels_);
}

TEST_F(ResamplerTestInterleaved, values_of_returned) {

    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;
    int fail_counter = 0;
    float limit = 3e-2;

    resampler_->SetSourceBuffer(resample_data_, length_);
    int returned_samples = resampler_->Resample(result, length_*2, ratio);

    for(int i=0; i<returned_samples; i++) {
        if(int diff = fabs(resample_expected_results_[i]-result[i]) > limit) {
            cout << "The difference between resample_expected_results_[i] and result[i] is " << diff
                << ", which exceeds limit"
                << ", where resample_expected_results_[i] evaluates to " << resample_expected_results_[i]
                << ", result[i] evaluates to " << result[i]
                << ", and limit evaluates to " << limit << endl
                << "vectors differ at indexes " << i << " (" << ++fail_counter << " fails)" << endl
                << "vectors can differ at max " << channels_ << " times" << endl;
        }

        if (fail_counter > channels_) {
            FAIL() << "TESTING ABORTED due to single vector difference";
        }
    }
}

TEST_F(ResamplerTestInterleaved, short_buffer_test) {
	
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*1];
    float ratio = 1.5;
    size_t ex_len = length_*ratio;

    resampler_->SetSourceBuffer(resample_data_, length_);
    int returned_samples = resampler_->Resample(result, length_, ratio);

    EXPECT_PRED3(DifferenceLimit, length_, returned_samples, channels_);

    returned_samples = resampler_->Resample(result, length_, ratio);

    EXPECT_PRED3(DifferenceLimit, ex_len-length_, returned_samples, channels_);
}

TEST(ResamplerRatioTest, variable_ratio_test) {
    const int data_length = 10;
    const int ratios_size = 3;

    float ratios[ratios_size] = {0.8, 0.95, 1.2};
    float data[data_length] = {0, 1, 0, -1, 0, 1, 0, -1, 0, 1};
    float result[data_length*2] = {};

    Resampler resampler = Resampler(SRC_SINC_MEDIUM_QUALITY, 1);

    for(int i=0; i<ratios_size; i++) {
        resampler.SetSourceBuffer(data, data_length);
        int returned_samples = resampler.Resample(result, data_length*2, ratios[i]);
        EXPECT_NE(0, returned_samples) << "process did not return any samples";
    }
}

TEST_F(ResamplerTestInterleaved, continuity_test) {
    ASSERT_TRUE(resample_data_loaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(resample_result_loaded_) << "TESTING CODE FAILED... could not load expected data";

    float result[length_*2];
    float ratio = 1.5;
    int fail_counter = 0;
    float limit = 4e-2;

    int block_len = 22;
    size_t returned_samples = 0;

    resampler_->SetSourceBuffer(resample_data_, length_);

    for(int j = 0; j < 100; j++){
        returned_samples = resampler_->Resample(result+j*block_len, block_len, ratio);
    }

    for(int i=0; i<returned_samples; i++) {
        if(float diff = fabs(resample_expected_results_[i]-result[i]) > limit) {
            printf("Expected result and calculated result do not match at index %d\n", i);
            printf("%.6f : %.6f \n",resample_expected_results_[i],result[i]);
            printf("Difference: %f\n", diff);
            printf("Failed %d time(s)\n",++fail_counter);
            printf("#####################\n");
        }
        if (fail_counter > 2) {
            FAIL();
        }
    }
}