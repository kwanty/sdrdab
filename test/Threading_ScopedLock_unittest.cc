#ifdef GOOGLE_UNIT_TEST

#include <pthread.h>

#include "gtest/gtest.h"
#include "threading/scoped_lock.h"

class ThreadingScopedLockTest  : public ::testing::Test{
    protected:
        ThreadingScopedLockTest(){
            pthread_mutex_init(&lock_, NULL);
        }

        virtual ~ThreadingScopedLockTest(){
            pthread_mutex_destroy(&lock_);
        }

        bool lock() {
            ScopedLock lock(lock_);
            return pthread_mutex_trylock(&lock_);
        }

        pthread_mutex_t lock_;
};


TEST_F(ThreadingScopedLockTest, locking) {
    // make sure it was locked after initializing
    EXPECT_NE(lock(), 0);
}

TEST_F(ThreadingScopedLockTest, unlocking) {
    lock();
    // make sure it was unlocked after leaving scope
    EXPECT_EQ(pthread_mutex_trylock(&lock_), 0);
}

#endif

