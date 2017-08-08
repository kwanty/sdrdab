/*
 * ring_buffer_test.cc
 *
 *  Created on: May 24, 2015
 *      Author: hipek
 */

#include "RingBuffer/ring_buffer.h"
#include "gtest/gtest.h"
#include "MatlabIO.h"

using namespace std;

class RingBufferTest: public testing::Test {
    public:

        RingBuffer<float> *ring_buffer;

        RingBufferTest(){
            ring_buffer = NULL;

        }
        virtual ~RingBufferTest(){

        }

        virtual void SetUp(){
            ring_buffer = new RingBuffer<float>(4);
        }

        virtual void TearDown(){
            delete ring_buffer;
        };


};

TEST_F(RingBufferTest, basic_func_test) {

    ring_buffer->WriteNext(2.2);
    ring_buffer->WriteNext(1.2);
    ring_buffer->WriteNext(2.2);
    ring_buffer->WriteNext(3.2);
    ring_buffer->WriteNext(4.2);
    ring_buffer->WriteNext(5.2);

    EXPECT_NEAR(14.8,ring_buffer->Sum(),1e-5);
    EXPECT_NEAR(3.7,ring_buffer->Mean(),1e-5);

    ring_buffer->Reset();
    EXPECT_EQ(0.0,ring_buffer->Sum());
    EXPECT_EQ(0,ring_buffer->DataStored());

    ring_buffer->Initialize(0.0);
    EXPECT_EQ(0.0,ring_buffer->Sum());
    EXPECT_EQ(4,ring_buffer->DataStored());


}

TEST_F(RingBufferTest, buffer_copying_test) {
    float in_buf[3] = {1.1, 2.2, 3.3};
    float out_buf[4];

    // writing lots of data
    for (int i=0;i<9;i++)
        ring_buffer->WriteInto(in_buf,3);

    // some data from rear some from front of "linear buffer"
    ring_buffer->ReadFrom(out_buf,4);

    EXPECT_NEAR(3.3,out_buf[0],1e-5);
    EXPECT_NEAR(1.1,out_buf[1],1e-5);
    EXPECT_NEAR(2.2,out_buf[2],1e-5);
    EXPECT_NEAR(3.3,out_buf[3],1e-5);
    EXPECT_EQ(0,ring_buffer->DataStored());


    //  exit(0);
}
