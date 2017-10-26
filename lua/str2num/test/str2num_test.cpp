/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Thu 26 Oct 2017 10:45:06 PM CST
 @File Name: str2num_test.cpp
 @Description:
 ******************************************************/

#include "../str2num.h"

#include <gtest/gtest.h>

TEST(STR2NUM, todouble) {
	const char *s = "19.123";
	double d;
	const char *r = l_str2d(s, &d);
	ASSERT_DOUBLE_EQ(d, 19.123);
	ASSERT_TRUE(r != NULL && *r == '\0');

	s = "  19.123   ";
	r = l_str2d(s, &d);
	ASSERT_DOUBLE_EQ(d, 19.123);
	ASSERT_TRUE(r != NULL && *r == '\0');

	s = "12.1abc";
	r = l_str2d(s, &d);
	ASSERT_TRUE(r == NULL);
}

TEST(STR2NUM, toint) {
	const char *s = "123456789087654321";
	long long i;
	const char *r = l_str2int(s, &i);
	ASSERT_EQ(i, 123456789087654321LL);
	ASSERT_TRUE(r != NULL && *r == '\0');

	s = "   123  ";
	r = l_str2int(s, &i);
	ASSERT_EQ(i, 123);
	ASSERT_TRUE(r != NULL && *r == '\0');

	s = "  0xa0  ";
	r = l_str2int(s, &i);
	ASSERT_EQ(i, 160);
	ASSERT_TRUE(r != NULL && *r == '\0');

	s = "123abc";
	r = l_str2int(s, &i);
	ASSERT_TRUE(r == NULL);

	s = "123.123";
	r = l_str2int(s, &i);
	ASSERT_TRUE(r == NULL);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
