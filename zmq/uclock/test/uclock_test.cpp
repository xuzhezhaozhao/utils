/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 15时48分35秒
 @File Name: clock_test.cpp
 @Description:
 ******************************************************/

#include "../uclock.hpp"

#include <gtest/gtest.h>

// 比较 now_us() 和 now_ms() 效率
static const int loop_times = 10000000;
TEST(CLOCK, now_us) {
	utils::uclock_t clock;
	for (int i = 0; i < loop_times; ++i) {
		clock.now_us();
	}
}

TEST(CLOCK, now_ms) {
	utils::uclock_t clock;
	for (int i = 0; i < loop_times; ++i) {
		clock.now_ms();
	}
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
