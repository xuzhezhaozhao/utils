/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月25日 星期五 22时50分58秒
 @File Name: str_convert.h
 @Description:
 ******************************************************/
#ifndef UTILS_STR_HELPER_STR_CONVERT_H_
#define UTILS_STR_HELPER_STR_CONVERT_H_

#include <string>

namespace utils {
namespace str_helper {

// helper from jsoncpp
std::string valueToString(int64_t value);
std::string valueToString(uint64_t value);
std::string valueToString(int value);
std::string valueToString(unsigned int value);
std::string valueToString(double value, bool useSpecialFloats,
							 unsigned int precision);


std::string valueToString(double value);
std::string valueToString(bool value);
std::string valueToQuotedString(const char* value);
inline std::string valueToQuotedString(const std::string& value) {
	return valueToQuotedString(value.data());
}
std::string valueToQuotedStringN(const char* value, unsigned length);

// 主要用在模板中
inline char valueToString(char value) { return value; }
inline const char* valueToString(const char* value) { return value; }
inline std::string valueToString(std::string value) { return value; }

}
}

#endif /* ifndef U */
