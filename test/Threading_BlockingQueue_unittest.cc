#ifdef GOOGLE_UNIT_TEST

#include "pthread.h"
#include "threading/blocking_queue.h"
#include "gtest/gtest.h"

TEST(ThreadingTest, BlockingQueueNotThreaded) {
    BlockingQueue<int> bq;
    int res;
    bq.push(1);
    bq.push(2);
    bq.push(3);
    bq.push(4);
    EXPECT_EQ(bq.pull(), 1) << "Pull returned wrong item.";
    bq.push(4);
    EXPECT_EQ(bq.pull(), 2) << "Pull returned wrong item.";
    EXPECT_EQ(bq.pull(), 3) << "Pull returned wrong item.";
    EXPECT_EQ(bq.pull(), 4) << "Pull returned wrong item.";
    EXPECT_EQ(bq.pull(), 4) << "Pull returned wrong item.";
    EXPECT_EQ(bq.size(), 0) << "Queue should be empty";
}

bool stop_producers = false;
bool stop_consumers = false;

void* producer(void *bq_) {
    BlockingQueue<int> *bq = reinterpret_cast<BlockingQueue<int>*>(bq_);
    while(!stop_producers) {
        bq->push(1);
        usleep(1000); // 1ms
    }
    return NULL;
}

void* consumer(void *bq_) {
    BlockingQueue<int> *bq = reinterpret_cast<BlockingQueue<int>*>(bq_);
    while(!stop_consumers) {
        bq->pull();
    }
    return NULL;
}

TEST(ThreadingTest, BlockingQueueThreadedSingleConsumer) {
    BlockingQueue<int> bq;
    pthread_t producers[4];
    pthread_t consumers[1];

    stop_producers = 0;
    stop_consumers = 0;
    pthread_create(&producers[0], NULL, producer, &bq);
    pthread_create(&producers[1], NULL, producer, &bq);
    pthread_create(&producers[2], NULL, producer, &bq);
    pthread_create(&producers[3], NULL, producer, &bq);
    // let producers produce messages into queue for 10ms
    usleep(1000*10);

    EXPECT_GT(bq.size(), 4);

    // start consumer after 10ms
    pthread_create(&consumers[0], NULL, consumer, &bq);

    // let producers and consumer work for 100ms
    usleep(1000*100);

    // stop producers
    stop_producers = 1;
    pthread_join(producers[0], NULL);
    pthread_join(producers[1], NULL);
    pthread_join(producers[2], NULL);
    pthread_join(producers[3], NULL);

    // wait 1ms to allow consumer to process remaining messages
    usleep(1000*1);

    // stop consumer
    stop_consumers = 1;
    // consumer will wait for one more message, provide it
    bq.push(1);
    pthread_join(consumers[0], NULL);

    EXPECT_EQ(bq.size(), 0);
}

TEST(ThreadingTest, BlockingQueueThreadedTwoConsumer) {
    stop_producers = 0;
    stop_consumers = 0;
    BlockingQueue<int> bq;
    pthread_t producers[4];
    pthread_t consumers[2];
    pthread_create(&producers[0], NULL, producer, &bq);
    pthread_create(&producers[1], NULL, producer, &bq);
    pthread_create(&producers[2], NULL, producer, &bq);
    pthread_create(&producers[3], NULL, producer, &bq);
    usleep(1000*10); // 10ms;

    EXPECT_GT(bq.size(), 4);

    pthread_create(&consumers[0], NULL, consumer, &bq);
    pthread_create(&consumers[1], NULL, consumer, &bq);

    usleep(1000*100); // 100ms
    stop_producers = 1;
    pthread_join(producers[0], NULL);
    pthread_join(producers[1], NULL);
    pthread_join(producers[2], NULL);
    pthread_join(producers[3], NULL);

    usleep(1000*10);
    stop_consumers = 1;
    bq.push(1);
    bq.push(1);
    pthread_join(consumers[0], NULL);
    pthread_join(consumers[1], NULL);

    EXPECT_EQ(bq.size(), 0);
}

#endif

