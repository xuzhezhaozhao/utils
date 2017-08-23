/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月23日 星期三 19时33分34秒
 @File Name: pb_helper.h
 @Description:
 ******************************************************/

#include <google/protobuf/message.h>
#include <string>

namespace utils 
{
namespace pb_helper
{

std::string pb2Json(const google::protobuf::Message *message);

}
}
