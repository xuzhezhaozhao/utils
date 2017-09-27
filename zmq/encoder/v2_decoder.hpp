/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 02:20:03 PM CST
 @File Name: v2_decoder.hpp
 @Description: from libzmq/src/v2_decoder.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_V2_DECODER_HPP_
#define UTILS_ZMQ_V2_DECODER_HPP_

#include "decoder.hpp"
#include "decoder_allocators/decoder_allocators.hpp"

namespace utils {
//  Decoder for ZMTP/2.x framing protocol. Converts data stream into messages.
//  The class has to inherit from shared_message_memory_allocator because
//  the base class calls allocate in its constructor.
class v2_decoder_t :
	// inherit first from allocator to ensure that it is constructed before
	// decoder_base_t
	public shared_message_memory_allocator,
	public decoder_base_t<v2_decoder_t, shared_message_memory_allocator> {
public:
	v2_decoder_t(size_t bufsize_, int64_t maxmsgsize_);
	virtual ~v2_decoder_t();

	//  i_decoder interface.
	virtual msg_t *msg() { return &in_progress; }

private:
	int flags_ready(unsigned char const *);
	int one_byte_size_ready(unsigned char const *);
	int eight_byte_size_ready(unsigned char const *);
	int message_ready(unsigned char const *);

	int size_ready(uint64_t size_, unsigned char const *);

	unsigned char tmpbuf[8];
	unsigned char msg_flags;
	msg_t in_progress;

	const int64_t maxmsgsize;

	v2_decoder_t(const v2_decoder_t &);
	void operator=(const v2_decoder_t &);
};
}

#endif
