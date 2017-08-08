/*
 * gtest_main.cc
 *
 *  Created on: 23 lut 2015
 *      Author: kwant
 */


#include <cstdio>
#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

