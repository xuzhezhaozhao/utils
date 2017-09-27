/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月24日 星期四 14时44分10秒
 @File Name: str_helper_test.cpp
 @Description:
 ******************************************************/

#include "../str_helper.hpp"
#include "gtest/gtest.h"

#include <climits>

using namespace utils::str_helper;
TEST(STR_STRING_HELPER, int) {
	ASSERT_STREQ(valueToString(0).data(), "0");
	ASSERT_STREQ(valueToString(1234567890).data(), "1234567890");
	ASSERT_STREQ(valueToString(-1234567890).data(), "-1234567890");
	ASSERT_STREQ(valueToString(INT_MAX).data(), "2147483647");
	ASSERT_STREQ(valueToString(INT_MIN).data(), "-2147483648");
}

TEST(STR_STRING_HELPER, int64_t) {
	ASSERT_STREQ(valueToString((int64_t)1234567890987654321).data(),
				 "1234567890987654321");
	ASSERT_STREQ(valueToString((int64_t)LLONG_MAX).data(),
				 "9223372036854775807");
	ASSERT_STREQ(valueToString((int64_t)LLONG_MIN).data(),
				 "-9223372036854775808");
}

TEST(STR_STRING_HELPER, uint) {
	ASSERT_STREQ(valueToString((unsigned int)0).data(), "0");
	ASSERT_STREQ(valueToString((unsigned int)123456789).data(), "123456789");
	ASSERT_STREQ(valueToString((unsigned int)UINT_MAX).data(), "4294967295");
}

TEST(STR_STRING_HELPER, uint64_t) {
	ASSERT_STREQ(valueToString((uint64_t)0).data(), "0");
	ASSERT_STREQ(valueToString((uint64_t)12345678987654321).data(),
				 "12345678987654321");
	ASSERT_STREQ(valueToString((uint64_t)ULLONG_MAX).data(),
				 "18446744073709551615");
}

TEST(STR_STRING_HELPER, bool) {
	ASSERT_STREQ(valueToString(true).data(), "true");
	ASSERT_STREQ(valueToString(false).data(), "false");
}

TEST(STR_STRING_HELPER, float) {
	ASSERT_STREQ(valueToString(-2147483648).data(), "-2147483648");
	ASSERT_STREQ(valueToString(1.0 / 3.0).data(), "0.33333333333333331");
	ASSERT_STREQ(valueToString(1.0 / 3.0, 0, 4).data(), "0.3333");
	ASSERT_STREQ(valueToString(1.0 / 3.0, 0, 10).data(), "0.3333333333");
	ASSERT_STREQ(valueToString(1.0 / 8.0, 0, 10).data(), "0.125");
	ASSERT_STREQ(valueToString(57.0 / 7.0, 0, 10).data(), "8.142857143");
	ASSERT_STREQ(valueToString(1058.9 / 7.2, 0, 2).data(), "1.5e+02");
	ASSERT_STREQ(valueToString(100262358.3159 / 723.252, 0, 5).data(),
				 "1.3863e+05");
	ASSERT_STREQ(valueToString(100262358232.3159 / 0.252, 0, 6).data(),
				 "3.97867e+11");

	ASSERT_STREQ(valueToString(1.0 / 0.0, 1, 4).data(), "Infinity");
	ASSERT_STREQ(valueToString(0.0 / 0.0, 1, 4).data(), "NaN");
}

TEST(STR_STRING_HELPER, json) {
	ASSERT_STREQ(valueToQuotedString("zhezhaoxu").data(), "\"zhezhaoxu\"");
	ASSERT_STREQ(valueToQuotedString("\"zhezhaoxu\" \"jingchenghuang\"").data(),
				 "\"\\\"zhezhaoxu\\\" \\\"jingchenghuang\\\"\"");
	ASSERT_STREQ(valueToQuotedString("徐哲钊").data(), "\"徐哲钊\"");
	ASSERT_STREQ(valueToQuotedString("\"徐哲钊\" \"jingchenghuang\"").data(),
				 "\"\\\"徐哲钊\\\" \\\"jingchenghuang\\\"\"");
	// TODO control character
}

TEST(STR_STRING_HELPER, pack_string) {
	ASSERT_STREQ(pack_string(1, 2, 3).data(), "123");
	ASSERT_STREQ(pack_string("abc", '-', "ABC").data(), "abc-ABC");
	ASSERT_STREQ(pack_string(std::string("abc"), '-', "ABC", 1, 2, 3).data(),
				 "abc-ABC123");
	short sh = 123;
	unsigned short ush = 123;
	ASSERT_STREQ(pack_string(1, '-', true, "-", 'a', '-', "abc", '-',
							 std::string("string"), '-', 123456789012372352ULL,
							 '-', -32, '-', 23456U, '-', 1234LL, '-', 123L, '-',
							 123UL, '-', sh, '-', ush, '\n', "徐哲钊")
					 .data(),
				 "1-true-a-abc-string-123456789012372352--32-23456-1234-123-"
				 "123-123-123\n徐哲钊");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
