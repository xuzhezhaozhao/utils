/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月25日 星期五 22时40分00秒
 @File Name: pack_string.h
 @Description:
 ******************************************************/
#ifndef UTILS_STRINGS_PACK_STRING_H
#define UTILS_STRINGS_PACK_STRING_H

#include "str_convert.h"
#include <iostream>

#include <string>
#include <tuple>

namespace {
using namespace utils::str_helper;


template <typename T, typename... Args>
struct PackString0 {
	static void pack_string0(std::string& result, T param, Args... args) {
		result += valueToString(param);
		PackString0<Args...>::pack_string0(result, args...);
	}
};
template <typename T>
struct PackString0<T> {
	static void pack_string0(std::string& result, T param) {
		result += valueToString(param);
	}
};

template <typename... Args>
struct PackString {
	static void pack_string0(std::string& result, Args... args) {
		PackString0<Args...>::pack_string0(result, args...);
	}
};

template <typename T>
void __pack_string0(std::string& result, T param) {
	result += valueToString(param);
}
template <typename T, typename... Args>
void __pack_string0(std::string& result, T param, Args... args) {
	result += valueToString(param);
	__pack_string0(result, args...);
}
} // namespace

namespace utils {
namespace str_helper {

template <typename... Args>
std::string pack_string(Args... args) {
	std::string result;
	//__pack_string0(result, args...);
	PackString<Args...>::pack_string0(result, args...);
	return result;
}

} // namespace str_helper
} // namespace utils

#endif /* ifndef */
