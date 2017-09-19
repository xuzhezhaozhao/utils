/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 19 Sep 2017 07:30:55 PM CST
 @File Name: likely.h
 @Description:
 ******************************************************/

#ifndef UTILS_LIKELY_H_
#define UTILS_LIKELY_H_

#if defined __GNUC__
#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#endif
