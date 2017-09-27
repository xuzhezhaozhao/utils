/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 01:33:26 PM CST
 @File Name: raw_decoder.hpp
 @Description: from libzmq/src/raw_decoder.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_RAW_DECODER_HPP_
#define UTILS_ZMQ_RAW_DECODER_HPP_

#include "decoder_allocators/decoder_allocators.hpp"
#include "error/err.h"
#include "i_decoder.hpp"
#include "msg/msg.hpp"

#include <stdint.h>

namespace utils {

//  Decoder for 0MQ v1 framing protocol. Converts data stream into messages.

class raw_decoder_t : public i_decoder {
public:
	raw_decoder_t(size_t bufsize_);
	virtual ~raw_decoder_t();

	//  i_decoder interface.

	virtual void get_buffer(unsigned char **data_, size_t *size_);

	virtual int decode(const unsigned char *data_, size_t size_,
					   size_t &processed);

	virtual msg_t *msg() { return &in_progress; }

	virtual void resize_buffer(size_t) {}

private:
	msg_t in_progress;

	shared_message_memory_allocator allocator;

	raw_decoder_t(const raw_decoder_t &);
	void operator=(const raw_decoder_t &);
};
}

#endif
