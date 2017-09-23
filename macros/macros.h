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

#endif

