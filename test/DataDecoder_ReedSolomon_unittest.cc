/*
============================================================================
Author      : Babiuch Michal
E-mail      : babiuch.michal@gmail.com
Date        : 26.05.2015

refactoring, adding test with errors:
Author      : Jaroslaw Bulat
E-mail      : kwant@agh.edu.pl
Date        : 26.10.2016

adding correctionCapability test:
Author      : Szymon Kurzepa
E-mail      : szymon.kurzepa@gmail.com
Date        : 11.02.2017
============================================================================
*/

#ifdef GOOGLE_UNIT_TEST

#include "data_decoder.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"
#include "myTimer.h"
#include <iostream> //!

using namespace std;

class RStest  : public ::testing::Test{
    protected:
	RStest() {
        param_.guard_size = 504;
        param_.fft_size = 2048;
        param_.symbol_size = 2552;
        param_.number_of_symbols = 76;
        param_.null_size = 2656;
        param_.frame_size = 196608;
        param_.number_of_carriers = 1536;
        param_.number_of_symbols_per_fic = 3;
        param_.number_of_fib = 12;
        param_.number_of_cif = 4;
        param_.number_of_deqpsk_unit_for_read = 5;
        param_.number_of_fib_per_cif = 3;
        param_.number_samp_after_timedep = 3096;
        param_.number_samp_after_vit = 768;
        param_.sync_read_size = 199264;
        param_.fic_size = 9216;
        param_.number_cu_per_symbol = 48;
        param_.number_symbols_per_cif = 18;
        param_.dab_mode = DAB_MODE_I;

        set1_inputLength_ = 1680;
        frameLength_ = 120;
        set1_outputLength_ = (set1_inputLength_/frameLength_)*(frameLength_-NPAR_);

        set1_inTruth_ = new uint8_t[set1_inputLength_];
        set1_outTruth_ = new uint8_t[set1_outputLength_];

        set1_inputLoaded_ = false;
        set1_outputLoaded_ = false;
		
		// 3rd test - correctionCapability
		t_ = 5;
		tries_ = 100;
		set2_length_ = (t_+2)*tries_*frameLength_;
		
		set2_inTruth_ = new uint8_t[set2_length_];
		set2_outTruth_ = new uint8_t[set2_length_];
		
		set2_inputLoaded_ = false;
		set2_outputLoaded_ = false;

        rs_ = new DataDecoder(&param_);
	}

	virtual ~RStest() {
        delete rs_;
		delete [] set1_inTruth_;
		delete [] set1_outTruth_;
		delete [] set2_inTruth_;
		delete [] set2_outTruth_;
	}

	virtual void SetUp() {
        set1_inputLoaded_ = MatlabIO::ReadData(set1_inTruth_, "./data/ut/daneRSWe.txt", set1_inputLength_);
        set1_outputLoaded_ = MatlabIO::ReadData(set1_outTruth_, "./data/ut/daneRSWy.txt", set1_outputLength_);
		
		set2_inputLoaded_ = MatlabIO::ReadData(set2_inTruth_, "./data/ut/rs_testData_3_in.txt", set2_length_);
		set2_outputLoaded_ = MatlabIO::ReadData(set2_outTruth_, "./data/ut/rs_testData_3_out.txt", set2_length_);
	}
    DataDecoder * rs_;

    static const int NPAR_ = 10;
	
	int t_;
	int tries_;
	
    bool set1_inputLoaded_;
    bool set1_outputLoaded_;
	bool set2_inputLoaded_;
	bool set2_outputLoaded_;

    int set1_inputLength_;
    int frameLength_;
    size_t set1_outputLength_;
	
	int set2_length_;

    uint8_t* set1_inTruth_;
    uint8_t* set1_outTruth_;
	uint8_t* set2_inTruth_;
	uint8_t* set2_outTruth_;

    ModeParameters param_;
};


TEST_F(RStest, singleRandomError) {
    ASSERT_TRUE(set1_inputLoaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(set1_outputLoaded_) << "TESTING CODE FAILED... could not load expected data";

	MyTimer mt;
	uint8_t tmpIn[set1_inputLength_];

	srand(time(NULL));

	// 1000 trials
	for(size_t t=0; t<1000; ++t){
		memcpy(tmpIn, set1_inTruth_, set1_inputLength_*sizeof(uint8_t));

		size_t bytesPosition = rand()%set1_inputLength_;
		size_t bitPosition = rand()%8;
		tmpIn[bytesPosition] ^= 1<<bitPosition;

        rs_-> ReedSolomonCorrection (tmpIn, set1_inputLength_);

		for(size_t i = 0; i < set1_outputLength_; i++) {
			EXPECT_EQ(tmpIn[i], set1_outTruth_[i]) << "Reed-Solomon correction differ on position: " << i;
		}
	}
}


TEST_F(RStest, correctionCapability) {
    ASSERT_TRUE(set2_inputLoaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(set2_outputLoaded_) << "TESTING CODE FAILED... could not load expected data";

    // ToDo: proff of error (modification in set2_outTruth_ not set2_inTruth_ !!!!
    // cout << "##### before: " << +set2_outTruth_[16307] << endl;
	int numberOfErrors = rs_->ReedSolomonCorrection(set2_inTruth_, set2_length_);
	EXPECT_EQ(numberOfErrors, tries_);
    // ToDo: proff of error (modification in set2_outTruth_ not set2_inTruth_ !!!!
	// cout << "##### after: " << +set2_outTruth_[16307] << endl;

	
	for(size_t i = 0; i < set2_length_-10*7*100; ++i) {
		EXPECT_EQ(set2_inTruth_[i], set2_outTruth_[i]) << "Reed-Solomon correction differ on position: " << i;
	}
}

// test speed of syndrome calculations
TEST_F(RStest, syndromeSpeedTest) {
    ASSERT_TRUE(set1_inputLoaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(set1_outputLoaded_) << "TESTING CODE FAILED... could not load expected data";

	MyTimer mt;
	uint8_t tmpIn[set1_inputLength_];

	// 1000 trials
	for(size_t k = 0; k < 10000; k++) {
		memcpy(tmpIn, set1_inTruth_, set1_inputLength_*sizeof(uint8_t));
		mt.tic();
		rs_-> ReedSolomonCorrection (tmpIn, set1_inputLength_);
		mt.tac2Buff();
	}

    cout << "Test of one symbol of 120 bytes (median): " << mt.medianMsfromBuff()/set1_inputLength_*frameLength_ << " [ms]" << endl;
}


// test correction speed 
TEST_F(RStest, correctionSpeedTest) {
    ASSERT_TRUE(set2_inputLoaded_) << "TESTING CODE FAILED... could not load expected data";
    ASSERT_TRUE(set2_outputLoaded_) << "TESTING CODE FAILED... could not load expected data";

	MyTimer mt;
	mt.tic();
	int numberOfErrors = rs_->ReedSolomonCorrection(set2_inTruth_, set2_length_);
	double roundsTime = mt.tacMs();

	cout << "Test of one symbol of 120 bytes (mean): " << roundsTime/set2_length_*frameLength_ << "[ms]" << endl;
}
#endif
