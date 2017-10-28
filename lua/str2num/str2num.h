/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Thu 26 Oct 2017 02:38:33 PM GMT
 @File Name: str2num.h
 @Description:
 ******************************************************/

#ifndef UTILS_LUA_STR2NUM_H_
#define UTILS_LUA_STR2NUM_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * 字符串转 double, 利用标准函数 strtod 实现,
 * 结果保存在 *result 中，失败(包括inf, nan)返回 NULL
 */
const char *l_str2d(const char *s, double *result);

/**
 * 字符串转整数，可处理16进制数，结果保存在 *result 中,
 * 失败返回NULL, 成功是指全部字符串都被处理了, 不是尽可能多的解析.
 */
const char *l_str2int(const char *s, long long *result);

#ifdef __cplusplus
}
#endif

#endif
