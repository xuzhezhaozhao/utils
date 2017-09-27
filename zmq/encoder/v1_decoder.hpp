/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 01:51:27 PM CST
 @File Name: v1_decoder.hpp
 @Description: from libzmq/src/v1_decoder.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_V1_DECODER_HPP
#define UTILS_ZMQ_V1_DECODER_HPP

#include "decoder.hpp"
#include "decoder_allocators/decoder_allocators.hpp"

namespace utils {
//  Decoder for ZMTP/1.0 protocol. Converts data batches into messages.

class v1_decoder_t : public utils::c_single_allocator,
					 public decoder_base_t<v1_decoder_t> {
public:
	v1_decoder_t(size_t bufsize_, int64_t maxmsgsize_);
	~v1_decoder_t();

	virtual msg_t *msg() { return &in_progress; }

private:
	int one_byte_size_ready(unsigned char const *);
	int eight_byte_size_ready(unsigned char const *);
	int flags_ready(unsigned char const *);
	int message_ready(unsigned char const *);

	unsigned char tmpbuf[8];
	msg_t in_progress;

	int64_t maxmsgsize;

	v1_decoder_t(const v1_decoder_t &);
	void operator=(const v1_decoder_t &);
};
}

#endif
