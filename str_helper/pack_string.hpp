/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月25日 星期五 22时40分00秒
 @File Name: pack_string.h
 @Description:
 @API:
 1. appendToString()
 2. pack_string()
 @Notes:
 ******************************************************/
#ifndef UTILS_STRINGS_PACK_STRING_H
#define UTILS_STRINGS_PACK_STRING_H

#include "str_convert.hpp"
#include <iostream>

#include <string>
#include <tuple>

namespace utils {
namespace str_helper {
using namespace utils::str_helper;
template <typename T>
void appendToString(std::string& result, T value) {
	result += valueToString(value);
}
void appendToString(std::string& result, char value) {
	result += value;
}
void appendToString(std::string& result, const char* value) {
	result += value;
}
void appendToString(std::string& result, const std::string value) {
	result += value;
}

} // namespace utils
} // namespace str_helper

namespace {
using namespace utils::str_helper;
template <typename T, typename... Args>
struct PackString0 {
	static void pack_string0(std::string& result, T param, Args... args) {
		appendToString(result, param);
		PackString0<Args...>::pack_string0(result, args...);
	}
};
template <typename T>
struct PackString0<T> {
	static void pack_string0(std::string& result, T param) {
		appendToString(result, param);
	}
};

template <typename T>
void __pack_string0(std::string& result, T param) {
	appendToString(result, param);
}
template <typename T, typename... Args>
void __pack_string0(std::string& result, T param, Args... args) {
	appendToString(result, param);
	__pack_string0(result, args...);
}
} // anonymous namespace

namespace utils {
namespace str_helper {

template <typename... Args>
std::string pack_string(Args... args) {
	std::string result;
	__pack_string0(result, args...);
	//PackString0<Args...>::pack_string0(result, args...);
	return result;
}

} // namespace str_helper
} // namespace utils

#endif /* ifndef */
