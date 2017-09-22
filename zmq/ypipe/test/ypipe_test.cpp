/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 00时19分35秒
 @File Name: ypipe_test.cpp
 @Description:
 ******************************************************/

#include "../ypipe.hpp"

#include <gtest/gtest.h>

TEST(YPIPE, queue) {
	int r = 0;
	bool ret = false;

	utils::ypipe_t<int, 64> ypipe;
	ypipe.write(1, false);
	ypipe.write(2, false);
	ypipe.write(3, false);
	ret = ypipe.read(&r);
	ASSERT_TRUE(!ret);

	ypipe.flush();
	ret = ypipe.read(&r);
	ASSERT_TRUE(ret);
	ASSERT_TRUE(r == 1);
	ret = ypipe.read(&r);
	ASSERT_TRUE(ret);
	ASSERT_TRUE(r == 2);
	ret = ypipe.read(&r);
	ASSERT_TRUE(ret);
	ASSERT_TRUE(r == 3);

	ret = ypipe.read(&r);
	ASSERT_TRUE(!ret);
}
