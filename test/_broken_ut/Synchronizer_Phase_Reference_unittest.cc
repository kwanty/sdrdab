/*
 * synchronizer_unittest.cc
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 */


#ifdef GOOGLE_UNIT_TEST

#include <cmath>
#include <cstdlib>
#include "synchronizer.h"
#include "fft_engine.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class Synchronizer_Phase_Reference  : public ::testing::Test{
protected:
	Synchronizer_Phase_Reference(){

		mode_1_parameters.null_size = 2656;
		mode_1_parameters.frame_size = 196608;
		mode_1_parameters.guard_size = 504;

		mode_1_parameters.fft_size = 2048;
		mode_1_parameters.number_of_carriers = 1536;
		mode_1_parameters.number_cu_per_symbol = 48;
		mode_1_parameters.dab_mode = DAB_MODE_I;

		mode_2_parameters.null_size = 2656;
		mode_2_parameters.frame_size = 196608;
		mode_2_parameters.guard_size = 126;

		mode_2_parameters.fft_size = 512;
		mode_2_parameters.number_of_carriers = 384;
		mode_2_parameters.number_cu_per_symbol = 12;
		mode_2_parameters.dab_mode = DAB_MODE_II;

		mode_3_parameters.null_size = 2656;
		mode_3_parameters.frame_size = 196608;
		mode_3_parameters.guard_size = 63;

		mode_3_parameters.fft_size = 256;
		mode_3_parameters.number_of_carriers = 192;
		mode_3_parameters.number_cu_per_symbol = 6;
		mode_3_parameters.dab_mode = DAB_MODE_III;

		mode_4_parameters.null_size = 2656;
		mode_4_parameters.frame_size = 196608;
		mode_4_parameters.guard_size = 252;

		mode_4_parameters.fft_size = 1024;
		mode_4_parameters.number_of_carriers = 768;
		mode_4_parameters.number_cu_per_symbol = 24;
		mode_4_parameters.dab_mode = DAB_MODE_IV;

		// phase reference generator
		phaseRef_len1 = 2552*2;
		phaseRef_len2 = 638*2;
		phaseRef_len3 = 319*2;
		phaseRef_len4 = 1276*2;

		phaseRefGen_output_loaded1 = false;
		phaseRefGen_output1 = new float[phaseRef_len1];
		phaseRefGen_output_loaded2 = false;
		phaseRefGen_output2 = new float[phaseRef_len2];
		phaseRefGen_output_loaded3 = false;
		phaseRefGen_output3 = new float[phaseRef_len3];
		phaseRefGen_output_loaded4 = false;
		phaseRefGen_output4 = new float[phaseRef_len4];

		syn_mode_1 = new Synchronizer(&mode_1_parameters, 400000);
		syn_mode_2 = new Synchronizer(&mode_2_parameters, 400000);
		syn_mode_3 = new Synchronizer(&mode_3_parameters, 400000);
		syn_mode_4 = new Synchronizer(&mode_4_parameters, 400000);
	}

	virtual ~Synchronizer_Phase_Reference(){
		delete [] phaseRefGen_output1;
		delete [] phaseRefGen_output2;
		delete [] phaseRefGen_output3;
		delete [] phaseRefGen_output4;
	}

	virtual void SetUp(){
		phaseRefGen_output_loaded1 = MatlabIO::ReadData( phaseRefGen_output1, "./test/testdata/synchronizer_phase_ref_symb_output1.txt", phaseRef_len1 );
		phaseRefGen_output_loaded2 = MatlabIO::ReadData( phaseRefGen_output2, "./test/testdata/synchronizer_phase_ref_symb_output2.txt", phaseRef_len2 );
		phaseRefGen_output_loaded3 = MatlabIO::ReadData( phaseRefGen_output3, "./test/testdata/synchronizer_phase_ref_symb_output3.txt", phaseRef_len3 );
		phaseRefGen_output_loaded4 = MatlabIO::ReadData( phaseRefGen_output4, "./test/testdata/synchronizer_phase_ref_symb_output4.txt", phaseRef_len4 );
	}

	float* PhaseReferenceGenTest(ModeParameters mode_parameters, float drift){
		size_t size = mode_parameters.number_of_carriers + 1;
		size_t ncarriers = mode_parameters.number_of_carriers / 2;

		float * fi = new float[size];
		float * phase_ref_values = new float[2 * size];

		if (mode_parameters.dab_mode == DAB_MODE_I)
			EvaluateFi(syn_mode_1->phase_ref_index_mode1, fi, mode_parameters);
		else if (mode_parameters.dab_mode == DAB_MODE_II)
			EvaluateFi(syn_mode_2->phase_ref_index_mode2, fi, mode_parameters);
		else if (mode_parameters.dab_mode == DAB_MODE_III)
			EvaluateFi(syn_mode_3->phase_ref_index_mode3, fi, mode_parameters);
		else if (mode_parameters.dab_mode == DAB_MODE_IV)
			EvaluateFi(syn_mode_4->phase_ref_index_mode4, fi, mode_parameters);

		for (size_t i = 0; i < 2 * size; i++){
			if (i == size)
				phase_ref_values[i] = 0;
			else if (i % 2)
				phase_ref_values[i] = sin(fi[i/2]);
			else
				phase_ref_values[i] = cos(fi[i/2]);
		}

		float * phase_ref_symb = new float[2*mode_parameters.fft_size];

		for (size_t i = 0; i < 2 * (ncarriers + 1); i++)
			phase_ref_symb[i] = phase_ref_values[2 * ncarriers + i];

		for (size_t i = 2 * (ncarriers + 1); i < 2 * mode_parameters.fft_size - 2 * ncarriers; i++)
			phase_ref_symb[i] = 0;

		for (size_t i = 2 * mode_parameters.fft_size - 1; i >= 2 * mode_parameters.fft_size - 2 * ncarriers; i--)
			phase_ref_symb[i] = phase_ref_values[2 * ncarriers - 2 * mode_parameters.fft_size + i];

		phase_ref_symb[0] = 0;

		float * sig_phase_ref_symb = new float[2 * mode_parameters.fft_size + 2 * mode_parameters.guard_size];

		FFTEngine* fftEngine = new FFTEngine(mode_parameters.fft_size);
		fftEngine->IFFT(phase_ref_symb);

		//Add cyclic prefix
		for (size_t i = 0; i < 2 * mode_parameters.guard_size; i++)
			sig_phase_ref_symb[i] = phase_ref_symb[2 * mode_parameters.fft_size - 2 * mode_parameters.guard_size + i];

		for (size_t i = 2 * mode_parameters.guard_size; i < 2 * mode_parameters.fft_size + 2 * mode_parameters.guard_size; i++)
			sig_phase_ref_symb[i] = phase_ref_symb[i - 2 * mode_parameters.guard_size];

		//Shift signal
		float * shifted_phase_ref= new float[2 * mode_parameters.fft_size + 2 * mode_parameters.guard_size];
		for (unsigned int i = 0; i<2 * mode_parameters.fft_size + 2 * mode_parameters.guard_size; i+=2){
			float e = 2*M_PI/(2*mode_parameters.fft_size)*i*drift;
			shifted_phase_ref[i]=sig_phase_ref_symb[i]*cos(e)-sig_phase_ref_symb[i+1]*sin(e);
			shifted_phase_ref[i+1]=sig_phase_ref_symb[i+1]*cos(e)+sig_phase_ref_symb[i]*sin(e);
		}

		sig_phase_ref_symb=shifted_phase_ref;

		//Deallocation
		delete [] phase_ref_values;
		delete [] fi;
		delete [] phase_ref_symb;
		delete fftEngine;

		return sig_phase_ref_symb;
	}

	void EvaluateFi(const int phase_ref_index_tab[][5], float* fi, ModeParameters mode_parameters) {
		size_t size = mode_parameters.number_of_carriers + 1;
		size_t ncarriers = mode_parameters.number_of_carriers / 2;

		for (size_t k = 0; k < mode_parameters.number_cu_per_symbol; k++) {
			for (int kk = phase_ref_index_tab[k][0];
					kk <= phase_ref_index_tab[k][1]; kk++) {
				if (kk + ncarriers == ((size - 1) / 2)) {
					fi[kk + ncarriers] = 0;
				} else {
					fi[kk + ncarriers] = (M_PI / 2)
                            		* (syn_mode_1->phase_parameter_h[phase_ref_index_tab[k][3]][kk
																								- phase_ref_index_tab[k][2]]
																								+ phase_ref_index_tab[k][4]);
				}
			}
		}
	}

	size_t phaseRef_len1, phaseRef_len2, phaseRef_len3, phaseRef_len4;
	bool phaseRefGen_output_loaded1, phaseRefGen_output_loaded2, phaseRefGen_output_loaded3, phaseRefGen_output_loaded4;
	float *phaseRefGen_output1, *phaseRefGen_output2, *phaseRefGen_output3, *phaseRefGen_output4;

	ModeParameters mode_1_parameters, mode_2_parameters, mode_3_parameters, mode_4_parameters;
	Synchronizer *syn_mode_1, *syn_mode_2, *syn_mode_3, *syn_mode_4;
	syncFeedback out_feedback;
};

//fc_short_search
TEST_F(Synchronizer_Phase_Reference, fc_short_search) {

	size_t size = 2 * mode_1_parameters.fft_size + 2 * mode_1_parameters.guard_size;
	syn_mode_1->fc_short_search_ = false;
	float drift;
	size_t i;
	//Simulate 30 frames with low fc_drift (-1e-2, 1e-2)
	for(i=1; i<30; i++){
		drift = -1e-2 + static_cast<float>(std::rand())/(static_cast<float>(RAND_MAX/2e-2));
		syn_mode_1->DetectPhaseReference(PhaseReferenceGenTest(mode_1_parameters, drift), size);
		EXPECT_NEAR(drift, syn_mode_1->fc_drift_, 1e-6);
		EXPECT_EQ(false, syn_mode_1->fc_short_search_);
		EXPECT_EQ(i, syn_mode_1->correct_frames_counter_ );
	}
	//Still low fc_drift
	//Test if fc_short_search will change
	//Test if fc_int and stripes are correct
	for(; i<60; i++){
		drift = -1e-2 + static_cast<float>(std::rand())/(static_cast<float>(RAND_MAX/2e-2));
		syn_mode_1->DetectPhaseReference(PhaseReferenceGenTest(mode_1_parameters, drift), size);
		EXPECT_NEAR(drift, syn_mode_1->fc_drift_, 1e-6);
		EXPECT_EQ(true, syn_mode_1->fc_short_search_);
		EXPECT_EQ(30, syn_mode_1->correct_frames_counter_ );
	}
	//Bigger fc_drift
	//Test if fc_short_search will be disabled
	for(; i<100; i++){
		drift = 1e-2 + static_cast<float>(std::rand())/(static_cast<float>(RAND_MAX/10));
		syn_mode_1->DetectPhaseReference(PhaseReferenceGenTest(mode_1_parameters, drift), size);
		EXPECT_NEAR(drift, syn_mode_1->fc_drift_, 1e-6);
		EXPECT_EQ(false, syn_mode_1->fc_short_search_);
		EXPECT_EQ(0, syn_mode_1->correct_frames_counter_ );
	}
}

//fc_drift tests
TEST_F(Synchronizer_Phase_Reference, fc_drift_mode_1) {

	size_t size = 2 * mode_1_parameters.fft_size + 2 * mode_1_parameters.guard_size;

	for(float i=-20; i<=20; i=i+0.1){
		syn_mode_1->DetectPhaseReference(PhaseReferenceGenTest(mode_1_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_1->fc_drift_, 1e-6);
	}
}

TEST_F(Synchronizer_Phase_Reference, fc_drift_mode_2) {

	size_t size = 2 * mode_2_parameters.fft_size + 2 * mode_2_parameters.guard_size;

	for(float i=-20; i<=20; i=i+0.1){
		syn_mode_2->DetectPhaseReference(PhaseReferenceGenTest(mode_2_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_2->fc_drift_, 1e-6);
	}
}

TEST_F(Synchronizer_Phase_Reference, fc_drift_mode_3) {

	size_t size = 2 * mode_3_parameters.fft_size + 2 * mode_3_parameters.guard_size;

	for(float i=-20; i<=20; i=i+0.1){
		syn_mode_3->DetectPhaseReference(PhaseReferenceGenTest(mode_3_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_3->fc_drift_, 1e-6);
	}
}
TEST_F(Synchronizer_Phase_Reference, fc_drift_mode_4) {

	size_t size = 2 * mode_4_parameters.fft_size + 2 * mode_4_parameters.guard_size;

	for(float i=-20; i<=20; i=i+0.1){
		syn_mode_4->DetectPhaseReference(PhaseReferenceGenTest(mode_4_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_4->fc_drift_, 1e-6);
	}
}

//fc_fract tests
TEST_F(Synchronizer_Phase_Reference, fc_fract_mode_1) {

	size_t size = 2 * mode_1_parameters.fft_size + 2 * mode_1_parameters.guard_size;

	for(float i=-1; i<=1; i=i+0.01){
		syn_mode_1->DetectPhaseReference(PhaseReferenceGenTest(mode_1_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_1->fc_drift_, 1e-6);
	}
}

TEST_F(Synchronizer_Phase_Reference, fc_fract_mode_2) {

	size_t size = 2 * mode_2_parameters.fft_size + 2 * mode_2_parameters.guard_size;

	for(float i=-1; i<=1; i=i+0.01){
		syn_mode_2->DetectPhaseReference(PhaseReferenceGenTest(mode_2_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_2->fc_drift_, 1e-6);
	}
}

TEST_F(Synchronizer_Phase_Reference, fc_fract_mode_3) {

	size_t size = 2 * mode_3_parameters.fft_size + 2 * mode_3_parameters.guard_size;

	for(float i=-1; i<=1; i=i+0.01){
		syn_mode_3->DetectPhaseReference(PhaseReferenceGenTest(mode_3_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_3->fc_drift_, 1e-6);
	}
}

TEST_F(Synchronizer_Phase_Reference, fc_fract_mode_4) {

	size_t size = 2 * mode_4_parameters.fft_size + 2 * mode_4_parameters.guard_size;

	for(float i=-1; i<=1; i=i+0.01){
		syn_mode_4->DetectPhaseReference(PhaseReferenceGenTest(mode_4_parameters, i), size);
		EXPECT_NEAR(i, syn_mode_4->fc_drift_, 1e-6);
	}
}

// Phase Reference Generator
//MODE 1
TEST_F(Synchronizer_Phase_Reference, phase_reference_mode_1) {
	ASSERT_TRUE(phaseRefGen_output_loaded1) << "TESTING CODE FAILED... could not load input data";

	float * sigPhaseRef = syn_mode_1->sigPhaseRef_;
	syn_mode_1->PhaseReferenceGen(sigPhaseRef);
	int fail_counter = 0;
	float error = 1e-7;

	for (size_t i = 0; i < phaseRef_len1; i++) {
		EXPECT_NEAR(phaseRefGen_output1[i], sigPhaseRef[i], error);

		if(abs(sigPhaseRef[i] - phaseRefGen_output1[i]) > error) {
			fail_counter++;
		}

		if(fail_counter >= 10) {
			FAIL() << "TESTING ABORTED due to massive vector difference";
		}
	}
}


//MODE 2
TEST_F(Synchronizer_Phase_Reference, phase_reference_mode_2) {
	ASSERT_TRUE(phaseRefGen_output_loaded2) << "TESTING CODE FAILED... could not load input data";

	float * sigPhaseRef = syn_mode_2->sigPhaseRef_;
	syn_mode_2->PhaseReferenceGen(sigPhaseRef );
	int fail_counter = 0;
	float error = 1e-7;

	for (size_t i = 0; i < phaseRef_len2; i++) {
		EXPECT_NEAR(phaseRefGen_output2[i], sigPhaseRef[i], error);

		if(abs(sigPhaseRef[i] - phaseRefGen_output2[i]) > error) {
			fail_counter++;
		}

		if(fail_counter >= 10) {
			FAIL() << "TESTING ABORTED due to massive vector difference";
		}
	}
}


//MODE 3
TEST_F(Synchronizer_Phase_Reference, phase_reference_mode_3) {
	ASSERT_TRUE(phaseRefGen_output_loaded3) << "TESTING CODE FAILED... could not load input data";

	float * sigPhaseRef = syn_mode_3->sigPhaseRef_;
	syn_mode_3->PhaseReferenceGen(sigPhaseRef );
	int fail_counter = 0;
	float error = 1e-7;

	for (size_t i = 0; i < phaseRef_len3; i++) {
		EXPECT_NEAR(phaseRefGen_output3[i], sigPhaseRef[i], error);

		if(abs(sigPhaseRef[i] - phaseRefGen_output3[i]) > error) {
			fail_counter++;
		}

		if(fail_counter >= 10) {
			FAIL() << "TESTING ABORTED due to massive vector difference";
		}
	}
}


//MODE 4
TEST_F(Synchronizer_Phase_Reference, phase_reference_mode_4) {
	ASSERT_TRUE(phaseRefGen_output_loaded4) << "TESTING CODE FAILED... could not load input data";

	float * sigPhaseRef = syn_mode_4->sigPhaseRef_;
	syn_mode_4->PhaseReferenceGen(sigPhaseRef );
	int fail_counter = 0;
	float error = 1e-7;

	for (size_t i = 0; i < phaseRef_len4; i++) {
		EXPECT_NEAR(phaseRefGen_output4[i], sigPhaseRef[i], error);
		if(abs(sigPhaseRef[i] - phaseRefGen_output4[i]) > error) {
			fail_counter++;
		}

		if(fail_counter >= 10) {
			FAIL() << "TESTING ABORTED due to massive vector difference";
		}
	}
}
#endif
