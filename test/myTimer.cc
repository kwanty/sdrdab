#include "myTimer.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iomanip>

#define DDD cout << "tick: " << __LINE__ << endl;
#define DD usleep( 1000*500 ); cout << "long tick: " << __LINE__ << endl;

using namespace std;

MyTimer::MyTimer( void ){
	clock_getres(CLOCK_MONOTONIC_RAW, &precision_);
}


MyTimer::~MyTimer( void ){
}

void MyTimer::printBuff(void) {
	printBuff(timeBuff_);
}

void MyTimer::printBuff(vector<struct timespec> timeBuff) {
	for( size_t i=0; i<timeBuff.size(); ++i ){
		long long int ns =  getNs(timeBuff[i]);
		cout << ns << " [ns], ";
		cout << fixed << setprecision(2) << static_cast<double>(ns)/1000.0 << " [us], ";
		cout << static_cast<double>(ns)/1e6 << " [ms], ";
		cout << ns/1e9 << "[s]" << endl;
	}
}

bool cmpTimeval(struct timespec x, struct timespec y) {
	// if x<y
	//	 return true
	// else
	//   return false
	if(x.tv_sec < y.tv_sec )
		return true;
	else if(x.tv_sec == y.tv_sec)
		if(x.tv_nsec<y.tv_nsec)
			return true;
	return false;
}


void MyTimer::printBuffSorted(void) {
	vector<struct timespec> sortedBuff(timeBuff_);
	size_t size = sortedBuff.size();
	if(size<2)
		return;

	double mean = meanMsfromBuff();
	double median = medianMsfromBuff();

	cout << "no.:" << sortedBuff.size() << " mean:" << mean << "[ms],  med:" << median << "[ms]" << endl;
	cout << "sorted values:" << endl;
	printBuff(sortedBuff);
}


double MyTimer::medianMsfromBuff(void){
    vector<struct timespec> sortedBuff(timeBuff_);
    size_t size = sortedBuff.size();
    if(size<2)
        return 0;

    sort(sortedBuff.begin(), sortedBuff.end(), cmpTimeval);

    double median;
    if (size  % 2 == 0) {
        median = static_cast<double>((getNs(sortedBuff[size / 2 - 1]) + getNs(sortedBuff[size / 2]))) / 2.0;
    }else{
        median = getNs(sortedBuff[size / 2]);
    }

    return median/1e6;              // ns -> ms conversion
}


double MyTimer::meanMsfromBuff(void){
    size_t size = timeBuff_.size();
    if(size<2)
        return 0;

    long long int sum = 0;
    for( size_t i=0; i<timeBuff_.size(); ++i )
        sum += getNs(timeBuff_[i]);

    return static_cast<double>(sum)/size/1e6; // ns -> ms conversion
}

double MyTimer::minimumMsfromBuff(void){
    size_t size = timeBuff_.size();
    if(size<2)
        return 0;

    long long int min = getNs(timeBuff_[0]);
    for(size_t i=1; i<timeBuff_.size(); ++i)
        if(getNs(timeBuff_[i]) < min)
            min = getNs(timeBuff_[i]);

    return min/1e6;         // ns -> ms conversion
}


long long int MyTimer::getNs(struct timespec time ){
	return static_cast<long long int>(time.tv_sec)*1e9 + static_cast<long long int>(time.tv_nsec);
}


void MyTimer::timespecdiff(struct timespec *x, struct timespec *y, struct timespec *diff){
	if(x->tv_sec<=y->tv_sec) {
		diff->tv_nsec = x->tv_nsec-y->tv_nsec;
	}else{
		diff->tv_nsec = x->tv_nsec-y->tv_nsec+1e9;
	}
	diff->tv_sec = x->tv_sec-y->tv_sec;
}
