/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 20 Sep 2017 07:55:11 PM CST
 @File Name: macros.h
 @Description:
 ******************************************************/

#ifndef UTILS_MACROS_H_
#define UTILS_MACROS_H_

#define UNUSED(object) (void)object
#define UTILS_DELETE(p_object) {\
    delete p_object; \
    p_object = 0; \
}


#if defined(__clang__) || defined(__GNUC__)
# define UTILS_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#elif defined(_MSC_VER)
# define UTILS_DEPRECATED(msg) __declspec(deprecated(msg))
#else
# define UTILS_DEPRECATED(msg)
#endif

#endif
