/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 12时52分30秒
 @File Name: signaler_test.cpp
 @Description:
 ******************************************************/

#include "../signaler.hpp"

#include <gtest/gtest.h>

TEST(SIGNALER, basic) {
	utils::signaler_t signaler;
	signaler.send();
	int rc = signaler.wait(0);
	ASSERT_TRUE( rc == 0 );
	signaler.recv();
}

// TODO use two threads

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
