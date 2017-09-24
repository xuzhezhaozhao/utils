/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月23日 星期六 22时53分14秒
 @File Name: blob.hpp
 @Description: from libzmq/src/blob.hpp
 ******************************************************/

#ifndef UTILS_BLOB_BLOB_H_
#define UTILS_BLOB_BLOB_H_

#include <string>

namespace utils {

//  Object to hold dynamically allocated opaque binary data.
typedef std::basic_string<unsigned char> blob_t;

}

#endif
