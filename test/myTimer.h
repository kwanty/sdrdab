/*
 * @class MyTimer
 * @brief for code debuging: measure time, colect statistics, etc..
 *
 * @author Jaroslaw Bulat kwant@agh.edu.pl
 * @version 2.0
 * @copyright Copyright (c) 2015 Jaroslaw Bulat
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
#ifndef MYTIMER_H_
#define MYTIMER_H_

#include <sys/time.h>
#include <vector>
#include <string>

using namespace std;

class MyTimer{
public:
	MyTimer(void);
	~MyTimer();

	/**
	 * start timer
	 */
	void tic(void);

	/**
	 * stop timer
	 * @return time in nano seconds [ns] between tic and tac
	 */
	long long int tacNs(void);

	/**
	 * stop timer
	 * @return time in seconds between [ms] tic and tac
	 */
	double tacMs(void);

	/**
	 * accumulate time interval between last tic and now
	 */
	void tac2Buff(void);

	/**
	 * print list of acquired time in order as was acquired
	 */
	void printBuff(void);

	/**
	 * print statistic and sorted list of acquired time
	 */
	void printBuffSorted(void);

    /**
     * returns median time [ms] from Buff
     */
    double medianMsfromBuff(void);

    /**
     * returns mean time [ms] from Buff
     */
    double meanMsfromBuff(void);

    /**
     * returns min time [ms] from Buff
     */
    double minimumMsfromBuff(void);


	/**
	 * return number of acquired time intervals
	 * @return number of acquired time intervals
	 */
	size_t bufferLength(void){return timeBuff_.size();}

private:
	struct timespec begin_;
	struct timespec precision_;
	vector<struct timespec> timeBuff_;

	void printBuff(vector<struct timespec> timeBuff);
	long long int getNs(struct timespec time);
	void timespecdiff(struct timespec *x, struct timespec *y, struct timespec *diff);
};


inline void MyTimer::tic( void )
{
	clock_gettime(CLOCK_MONOTONIC_RAW, &begin_);
}

inline long long int MyTimer::tacNs( void )
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);

	struct timespec diff;
	timespecdiff(&now, &begin_, &diff);
//	cout << begin_.tv_sec << " " << now.tv_sec << endl;
//	cout << begin_.tv_nsec << " " << now.tv_nsec << " :" <<  now.tv_nsec-begin_.tv_nsec << endl;
//	cout << diff.tv_sec << " " << diff.tv_nsec << " " << getNs(diff) << endl << endl;

	return getNs(diff);
}

inline double MyTimer::tacMs( void )
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);

	struct timespec diff;
	timespecdiff(&now, &begin_, &diff);

	return static_cast<double>(diff.tv_sec)*1e3 + static_cast<double>(diff.tv_nsec)/1e6;
}

inline void MyTimer::tac2Buff(void) {
	struct timespec now, diff;

	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	timespecdiff(&now, &begin_, &diff);
	timeBuff_.push_back(diff);
}

#endif /*MYTIMER_H_*/
