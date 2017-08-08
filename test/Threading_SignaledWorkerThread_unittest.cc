#ifdef GOOGLE_UNIT_TEST

#include <pthread.h>

#include "gtest/gtest.h"
#include "threading/blocking_queue.h"
#include "threading/signaled_worker_thread.h"

void worker_process(void *x) {
    int *xi = reinterpret_cast<int*>(x);
    ++*xi;
}

TEST(ThreadingTest, SignaledWorkerThread) {
    BlockingQueue<int> queue;
    SignaledWorkerThread worker;
    int x = 0;

    worker.init(&worker_process, &x, 0, &queue);

    // first pass
    worker.resume_thread();
    usleep(1000*1); // sleep 1ms
    ASSERT_EQ(queue.size(), 1);
    queue.pull();
    EXPECT_EQ(x, 1);

    // second pass
    worker.resume_thread();
    usleep(1000*1); // sleep 1ms
    ASSERT_EQ(queue.size(), 1);
    queue.pull();
    EXPECT_EQ(x, 2);
}

TEST(ThreadingTest, UninitializedIsSuspendedTest) {
    SignaledWorkerThread worker;
    ASSERT_TRUE(worker.is_suspended()) << "Not initialized thread should say it's suspended!";
}

#endif
