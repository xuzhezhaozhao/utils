/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 02:04:09 PM CST
 @File Name: v1_encoder.cpp
 @Description: from libzmq/src/v1_encoder.cpp
 ******************************************************/

#include "v1_encoder.hpp"
#include "encoder.hpp"
#include "likely/likely.h"
#include "wire/wire.hpp"

utils::v1_encoder_t::v1_encoder_t(size_t bufsize_)
	: encoder_base_t<v1_encoder_t>(bufsize_) {
	//  Write 0 bytes to the batch and go to message_ready state.
	next_step(NULL, 0, &v1_encoder_t::message_ready, true);
}

utils::v1_encoder_t::~v1_encoder_t() {}

void utils::v1_encoder_t::size_ready() {
	//  Write message body into the buffer.
	next_step(in_progress->data(), in_progress->size(),
			  &v1_encoder_t::message_ready, true);
}

void utils::v1_encoder_t::message_ready() {
	//  Get the message size.
	size_t size = in_progress->size();

	//  Account for the 'flags' byte.
	size++;

	//  For messages less than 255 bytes long, write one byte of message size.
	//  For longer messages write 0xff escape character followed by 8-byte
	//  message size. In both cases 'flags' field follows.
	if (size < 255) {
		tmpbuf[0] = (unsigned char)size;
		tmpbuf[1] = (in_progress->flags() & msg_t::more);
		next_step(tmpbuf, 2, &v1_encoder_t::size_ready, false);
	} else {
		tmpbuf[0] = 0xff;
		put_uint64(tmpbuf + 1, size);
		tmpbuf[9] = (in_progress->flags() & msg_t::more);
		next_step(tmpbuf, 10, &v1_encoder_t::size_ready, false);
	}
}
