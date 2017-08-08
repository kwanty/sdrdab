/*
 * myTimer_unittest.cc
 *
 *  Created on: 20 wrz 2015
 *      Author: kwant
 *
 * tips: https://code.google.com/p/googletest/wiki/FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog
 */


#ifdef GOOGLE_UNIT_TEST

#include "gtest/gtest.h"
#include "myTimer.h"
#include <unistd.h>

using namespace std;

class MyTimerTest  : public ::testing::Test{
protected:
	MyTimerTest(){

	}

	virtual ~MyTimerTest(){
	}

	virtual void SetUp(){
	}
};


TEST_F(MyTimerTest, single_us) {

	MyTimer mt;
	long long int result;
	mt.tic();
	usleep(1);
	long long int penalty = mt.tacNs()-1;

	for( size_t i=100; i<10000; i+=100){
		mt.tic();
		usleep(i);
		result = mt.tacNs();
		EXPECT_NEAR(i, result-penalty, 2*penalty+i/10) << "why wrong? ::: i=" << i << " result=" << result << " penalty+10%=" << 2*penalty+i/10;
	}
}

TEST_F(MyTimerTest, single_ms) {

	MyTimer mt;
	double result;
	mt.tic();
	usleep(1000);
	double penalty = mt.tacMs()-1.0;

	for( size_t i=1; i<100; ++i){
		mt.tic();
		usleep(i*1000);
		result = mt.tacMs();
		EXPECT_NEAR(i, result-penalty, 2*penalty+i/10) << "why wrong? ::: i=" << i << " result=" << result << " penalty+10%=" << 2*penalty+i/10;
	}
}

TEST_F(MyTimerTest, multiple_odd) {
	MyTimer mt;

	mt.tic();
	usleep(10000);
	mt.tac2Buff();

	mt.tic();
	usleep(100);
	mt.tac2Buff();

	mt.tic();
	usleep(1000);
	mt.tac2Buff();

	mt.tic();
	usleep(1000);
	mt.tac2Buff();

	mt.tic();
	usleep(1000);
	mt.tac2Buff();


	cout << endl << endl;
	mt.printBuff();

	cout << endl;
	mt.printBuffSorted();
	cout << endl << endl;
}

TEST_F(MyTimerTest, multiple_even) {
	MyTimer mt;

	mt.tic();
	usleep(10000);
	mt.tac2Buff();

	mt.tic();
	usleep(100);
	mt.tac2Buff();

	mt.tic();
	usleep(1000);
	mt.tac2Buff();

	mt.tic();
	usleep(1000);
	mt.tac2Buff();


	cout << endl << endl;
	mt.printBuff();

	cout << endl;
	mt.printBuffSorted();
	cout << endl << endl;
}


TEST_F(MyTimerTest, multiple_long) {
	MyTimer mt;

	mt.tic();
	usleep(1e6);
	mt.tac2Buff();

	mt.tic();
	usleep(1e5);
	mt.tac2Buff();

	mt.tic();
	usleep(3e5);
	mt.tac2Buff();

	mt.tic();
	usleep(9e5);
	mt.tac2Buff();


	cout << endl << endl;
	mt.printBuff();

	cout << endl;
	mt.printBuffSorted();
	cout << endl << endl;
}

#endif
