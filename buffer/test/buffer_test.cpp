/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 31 Oct 2017 03:41:04 PM CST
 @File Name: buffer_test.cpp
 @Description:
 ******************************************************/

#include "../buffer.h"

#include <gtest/gtest.h>

#include <string.h>

TEST(BUFFER_TEST, init) {
	utils::Buffer buffer;
	ASSERT_TRUE(buffer.used() == 0);
}

TEST(BUFFER_TEST, addstring) {
	utils::Buffer buffer;
	const char* s = "abcdef";
	size_t len = strlen(s);
	buffer.addstring(s, len);
	ASSERT_TRUE(buffer.used() == len);
	ASSERT_TRUE(buffer.size() >= len);
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.data() != NULL);
	ASSERT_TRUE(memcmp(buffer.data(), s, len) == 0);

	std::cout << "buffer clear..." << std::endl;
	buffer.clear();
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.used() == 0);

	buffer.addstring(s, len);
	ASSERT_TRUE(buffer.used() == len);
	ASSERT_TRUE(buffer.size() >= len);
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.data() != NULL);
	ASSERT_TRUE(memcmp(buffer.data(), s, len) == 0);
}

TEST(BUFFER_TEST, addstring_times) {
	utils::Buffer buffer;
	const char* s = "abcdef";
	size_t len = strlen(s);

	const int times = 10000;
	for (int i = 0; i < times; ++i) {
		buffer.addstring(s, len);
	}
	size_t expect_sz = len * times;
	ASSERT_TRUE(buffer.used() == expect_sz);
	ASSERT_TRUE(buffer.size() >= expect_sz);
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.data() != NULL);

	std::string expect_str;
	for (int i = 0; i < times; ++i) {
		expect_str += s;
	}
	ASSERT_TRUE(memcmp(buffer.data(), expect_str.data(), expect_str.size()) == 0);

	std::cout << "buffer clear..." << std::endl;
	buffer.clear();
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.used() == 0);

	for (int i = 0; i < times; ++i) {
		buffer.addstring(s, len);
	}
	ASSERT_TRUE(buffer.used() == expect_sz);
	ASSERT_TRUE(buffer.size() >= expect_sz);
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.data() != NULL);
	ASSERT_TRUE(memcmp(buffer.data(), expect_str.data(), expect_str.size()) == 0);
}

TEST(BUFFER_TEST, addstring_large) {
	utils::Buffer buffer;
	const char* s = "abcdef";
	size_t len = strlen(s);

	const int times = 10000;
	size_t expect_sz = len * times;
	std::string expect_str;
	for (int i = 0; i < times; ++i) {
		expect_str += s;
	}

	buffer.addstring(expect_str.data(), expect_str.size());

	ASSERT_TRUE(buffer.used() == expect_sz);
	ASSERT_TRUE(buffer.size() >= expect_sz);
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.data() != NULL);
	ASSERT_TRUE(memcmp(buffer.data(), expect_str.data(), expect_str.size()) == 0);

	std::cout << "buffer clear..." << std::endl;
	buffer.clear();
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.used() == 0);
	buffer.addstring(expect_str.data(), expect_str.size());

	ASSERT_TRUE(buffer.used() == expect_sz);
	ASSERT_TRUE(buffer.size() >= expect_sz);
	std::cout << "buffer.size() = " << buffer.size() << std::endl;
	ASSERT_TRUE(buffer.data() != NULL);
	ASSERT_TRUE(memcmp(buffer.data(), expect_str.data(), expect_str.size()) == 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
