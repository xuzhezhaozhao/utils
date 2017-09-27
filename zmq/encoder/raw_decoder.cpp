/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 01:34:33 PM CST
 @File Name: raw_decoder.cpp
 @Description: from libzmq/src/raw_decoder.cpp
 ******************************************************/

#include <stdlib.h>
#include <string.h>

#include "error/err.h"
#include "raw_decoder.hpp"

utils::raw_decoder_t::raw_decoder_t(size_t bufsize_) : allocator(bufsize_, 1) {
	int rc = in_progress.init();
	errno_assert(rc == 0);
}

utils::raw_decoder_t::~raw_decoder_t() {
	int rc = in_progress.close();
	errno_assert(rc == 0);
}

void utils::raw_decoder_t::get_buffer(unsigned char **data_, size_t *size_) {
	*data_ = allocator.allocate();
	*size_ = allocator.size();
}

int utils::raw_decoder_t::decode(const uint8_t *data_, size_t size_,
								 size_t &bytes_used_) {
	int rc = in_progress.init((unsigned char *)data_, size_,
							  shared_message_memory_allocator::call_dec_ref,
							  allocator.buffer(), allocator.provide_content());

	// if the buffer serves as memory for a zero-copy message, release it
	// and allocate a new buffer in get_buffer for the next decode
	if (in_progress.is_zcmsg()) {
		allocator.advance_content();
		allocator.release();
	}

	errno_assert(rc != -1);
	bytes_used_ = size_;
	return 1;
}
