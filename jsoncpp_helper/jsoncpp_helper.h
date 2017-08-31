/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月31日 星期四 22时03分51秒
 @File Name: jsoncpp_helper.h
 @Description:
 ******************************************************/

#ifndef UTILS_JSONCPP_HELPER_H_
#define UTILS_JSONCPP_HELPER_H_

#include "json/json.h"

namespace jsoncpp_helper {

struct null_tag_t {};
struct bool_tag_t {};
struct int32_tag_t {};
struct int64_tag_t {};
struct uint32_tag_t {};
struct uint64_tag_t {};
struct integral_tag_t {};
struct double_tag_t {};
struct numeric_tag_t {};
struct string_tag_t {};
struct array_tag_t {};
struct object_tag_t {};

const struct null_tag_t null_tag;
const struct bool_tag_t bool_tag;
const struct int32_tag_t int32_tag;
const struct int64_tag_t int64_tag;
const struct uint32_tag_t uint32_tag;
const struct uint64_tag_t uint64_tag;
const struct integral_tag_t integral_tag;
const struct double_tag_t double_tag;
const struct numeric_tag_t numeric_tag;
const struct string_tag_t string_tag;
const struct array_tag_t array_tag;
const struct object_tag_t object_tag;

bool __checkJsoncppArgs(const Json::Value&) { return true; }

#define DEFINE_checkJsoncppArgs(type1, type2)                                 \
	template <typename... Args>                                               \
	bool __checkJsoncppArgs(const Json::Value& value, const char* key, type1, \
							Args... args) {                                   \
		if (!(value.isMember(key) && value[key].type2())) {                   \
			return false;                                                     \
		}                                                                     \
		return __checkJsoncppArgs(value, args...);                            \
	}

DEFINE_checkJsoncppArgs(null_tag_t, isNull);
DEFINE_checkJsoncppArgs(bool_tag_t, isBool);
DEFINE_checkJsoncppArgs(int32_tag_t, isInt);
DEFINE_checkJsoncppArgs(int64_tag_t, isInt64);
DEFINE_checkJsoncppArgs(uint32_tag_t, isUInt);
DEFINE_checkJsoncppArgs(uint64_tag_t, isUInt64);
DEFINE_checkJsoncppArgs(integral_tag_t, isIntegral);
DEFINE_checkJsoncppArgs(double_tag_t, isDouble);
DEFINE_checkJsoncppArgs(numeric_tag_t, isNumeric);
DEFINE_checkJsoncppArgs(string_tag_t, isString);
DEFINE_checkJsoncppArgs(array_tag_t, isArray);
DEFINE_checkJsoncppArgs(object_tag_t, isObject);

// 检查value中字段类型, 使用方法如下
// checkJsoncppArgs(value, key1, int32_tag, key2, string_tag);
template <typename... Args>
bool checkJsoncppArgs(const Json::Value& value, Args... args) {
	return __checkJsoncppArgs(value, args...);
}

};

#endif
