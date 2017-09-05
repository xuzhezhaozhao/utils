/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月31日 星期四 22时03分51秒
 @File Name: jsoncpp_helper.h
 @Description:
 提供如下功能
 1. 方便检查 jsoncpp Json::Value 对象的字段类型
 ******************************************************/

#ifndef UTILS_JSONCPP_HELPER_H_
#define UTILS_JSONCPP_HELPER_H_

#include "json/json.h"

namespace utils {
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

// initialize to suppress compile error for gcc 4.4
static const struct null_tag_t null_tag = {};
static const struct bool_tag_t bool_tag = {};
static const struct int32_tag_t int32_tag = {};
static const struct int64_tag_t int64_tag = {};
static const struct uint32_tag_t uint32_tag = {};
static const struct uint64_tag_t uint64_tag = {};
static const struct integral_tag_t integral_tag = {};
static const struct double_tag_t double_tag = {};
static const struct numeric_tag_t numeric_tag = {};
static const struct string_tag_t string_tag = {};
static const struct array_tag_t array_tag = {};
static const struct object_tag_t object_tag = {};

// base
bool __checkJsonArgs(const Json::Value&) { return true; }

#define DEFINE_checkJsonArgs(type1, type2)                                 \
	template <typename... Args>                                               \
	bool __checkJsonArgs(const Json::Value& value, const char* key, type1, \
							Args... args) {                                   \
		if (!(value.isMember(key) && value[key].type2())) {                   \
			return false;                                                     \
		}                                                                     \
		return __checkJsonArgs(value, args...);                            \
	}

DEFINE_checkJsonArgs(null_tag_t, isNull);
DEFINE_checkJsonArgs(bool_tag_t, isBool);
DEFINE_checkJsonArgs(int32_tag_t, isInt);
DEFINE_checkJsonArgs(int64_tag_t, isInt64);
DEFINE_checkJsonArgs(uint32_tag_t, isUInt);
DEFINE_checkJsonArgs(uint64_tag_t, isUInt64);
DEFINE_checkJsonArgs(integral_tag_t, isIntegral);
DEFINE_checkJsonArgs(double_tag_t, isDouble);
DEFINE_checkJsonArgs(numeric_tag_t, isNumeric);
DEFINE_checkJsonArgs(string_tag_t, isString);
DEFINE_checkJsonArgs(array_tag_t, isArray);
DEFINE_checkJsonArgs(object_tag_t, isObject);

template <typename T>
struct is_jsontype {
	enum { value = 0 };
};
template <>
struct is_jsontype<bool> {
	enum { value = 1 };
};
template <>
struct is_jsontype<int32_t> {
	enum { value = 1 };
};
template <>
struct is_jsontype<uint32_t> {
	enum { value = 1 };
};
template <>
struct is_jsontype<int64_t> {
	enum { value = 1 };
};
template <>
struct is_jsontype<uint64_t> {
	enum { value = 1 };
};
template <>
struct is_jsontype<double> {
	enum { value = 1 };
};
template <>
struct is_jsontype<const char*> {
	enum { value = 1 };
};
template <>
struct is_jsontype<Json::Value> {
	enum { value = 1 };
};

// base
void __setJsonValue(Json::Value&) {}

template <typename T, typename... Args,
		  typename = typename std::enable_if<is_jsontype<
			  typename std::remove_reference<T>::type>::value>::type>
void __setJsonValue(Json::Value& value, const char* key, T&& val,
					   Args... args) {
	value[key] = std::forward<T>(val);
	__setJsonValue(value, args...);
}

// base
void __setJsonArrayValue(Json::Value&) {}

template <typename T, typename... Args,
		  typename = typename std::enable_if<is_jsontype<
			  typename std::remove_reference<T>::type>::value>::type>
void __setJsonArrayValue(Json::Value& arrayValue, T&& val, Args... args) {
	arrayValue[ arrayValue.size() ] = std::forward<T>(val);
	__setJsonArrayValue(arrayValue, args...);
}

// 利用 SFINAE
template <typename T>
void __setJsonArrayValue(Json::Value& arrayValue, T input, size_t N) {
	for (Json::ArrayIndex index = 0; index < N; ++index) {
		arrayValue[index] = input[index];
	}
}

/********* API ****************************/

// 检查value中字段类型, 使用方法如下
// checkJsonArgs(value, key1, int32_tag, key2, string_tag);
template <typename... Args>
bool checkJsonArgs(const Json::Value& value, Args... args) {
	return __checkJsonArgs(value, args...);
}

// 设置 value 中的字段，使用方法如下：
// setJsonValue(value, key1, val1, key2, val2, key3, val3, ...);
// key* 为字符串类型， val* 为 jsoncpp 支持的任意类型
template <typename... Args>
void setJsonValue(Json::Value& value, Args... args) {
	__setJsonValue(value, args...);
}


// 设置数组类型的值，使用方法如下：
// 有两种形式：
// 1. setJsonArrayValue(arrayValue, val1, val2, val3, ...);
//  val* 为 jsoncpp 支持的任意类型
// 2. setJsonArrayValue(arrayValue, input, N);
//  input 类型只要重载了 [] 操作符就行，N 为数组大小
template <typename... Args>
void setJsonArrayValue(Json::Value& arrayValue, Args... args) {
	__setJsonArrayValue(arrayValue, args...);
}

/******** end API ***********************/
}
}

#endif
