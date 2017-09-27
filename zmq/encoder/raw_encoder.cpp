/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 12:59:02 PM CST
 @File Name: raw_encoder.cpp
 @Description: from libzmq/src/raw_encoder.cpp
 ******************************************************/

#include "raw_encoder.hpp"
#include "encoder.hpp"
#include "likely/likely.h"

utils::raw_encoder_t::raw_encoder_t(size_t bufsize_)
	: encoder_base_t<raw_encoder_t>(bufsize_) {
	//  Write 0 bytes to the batch and go to message_ready state.
	next_step(NULL, 0, &raw_encoder_t::raw_message_ready, true);
}

utils::raw_encoder_t::~raw_encoder_t() {}

void utils::raw_encoder_t::raw_message_ready() {
	next_step(in_progress->data(), in_progress->size(),
			  &raw_encoder_t::raw_message_ready, true);
}
