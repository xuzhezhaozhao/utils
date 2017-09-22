/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月23日 星期三 19时33分34秒
 @File Name: pb_helper.hpp
 @Description:
 1. protobuf 转 json 字符串
 ******************************************************/
#ifndef UTILS_PB_PB_HELPER_H_
#define UTILS_PB_PB_HELPER_H_

#include <google/protobuf/message.h>
#include <string>

namespace utils 
{
namespace pb_helper
{

// oops, 造轮子, protobuf::util 中已经实现了该功能
std::string protobufToJsonString(const google::protobuf::Message* message);

}
}

#endif /* ifndef UTILS_PB_PB_HELPER_H_ */
