/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 15时08分53秒
 @File Name: str_helper.h
 @Description: 字符串操作
 ******************************************************/

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
std::string valueToQuotedStringN(const char* value, unsigned length);

std::string pack_string();

}
}
