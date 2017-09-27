/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 02:22:42 PM CST
 @File Name: v2_protocol.hpp
 @Description: from libzmq/src/v2_protocol.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_V2_PROTOCOL_HPP_
#define UTILS_ZMQ_V2_PROTOCOL_HPP_

namespace utils {
//  Definition of constants for ZMTP/2.0 transport protocol.
class v2_protocol_t {
public:
	//  Message flags.
	enum { 
		more_flag = 1, 
		large_flag = 2,
		command_flag = 4 
	};
};
}

#endif
