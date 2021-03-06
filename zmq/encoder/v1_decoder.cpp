/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 02:00:48 PM CST
 @File Name: v1_decoder.cpp
 @Description: from libzmq/src/v1_decoder.cpp
 ******************************************************/

#include <stdlib.h>
#include <string.h>
#include <limits>

#include "decoder.hpp"
#include "error/err.h"
#include "likely/likely.h"
#include "v1_decoder.hpp"
#include "wire/wire.hpp"

utils::v1_decoder_t::v1_decoder_t(size_t bufsize_, int64_t maxmsgsize_)
	: c_single_allocator(bufsize_),
	  decoder_base_t<v1_decoder_t>(this),
	  maxmsgsize(maxmsgsize_) {
	int rc = in_progress.init();
	errno_assert(rc == 0);

	//  At the beginning, read one byte and go to one_byte_size_ready state.
	next_step(tmpbuf, 1, &v1_decoder_t::one_byte_size_ready);
}

utils::v1_decoder_t::~v1_decoder_t() {
	int rc = in_progress.close();
	errno_assert(rc == 0);
}

int utils::v1_decoder_t::one_byte_size_ready(unsigned char const*) {
	//  First byte of size is read. If it is 0xff read 8-byte size.
	//  Otherwise allocate the buffer for message data and read the
	//  message data into it.
	if (*tmpbuf == 0xff)
		next_step(tmpbuf, 8, &v1_decoder_t::eight_byte_size_ready);
	else {
		//  There has to be at least one byte (the flags) in the message).
		if (!*tmpbuf) {
			errno = EPROTO;
			return -1;
		}

		if (maxmsgsize >= 0 && (int64_t)(*tmpbuf - 1) > maxmsgsize) {
			errno = EMSGSIZE;
			return -1;
		}

		int rc = in_progress.close();
		assert(rc == 0);
		rc = in_progress.init_size(*tmpbuf - 1);
		if (rc != 0) {
			errno_assert(errno == ENOMEM);
			rc = in_progress.init();
			errno_assert(rc == 0);
			errno = ENOMEM;
			return -1;
		}

		next_step(tmpbuf, 1, &v1_decoder_t::flags_ready);
	}
	return 0;
}

int utils::v1_decoder_t::eight_byte_size_ready(unsigned char const*) {
	//  8-byte payload length is read. Allocate the buffer
	//  for message body and read the message data into it.
	const uint64_t payload_length = get_uint64(tmpbuf);

	//  There has to be at least one byte (the flags) in the message).
	if (payload_length == 0) {
		errno = EPROTO;
		return -1;
	}

	//  Message size must not exceed the maximum allowed size.
	if (maxmsgsize >= 0 && payload_length - 1 > (uint64_t)maxmsgsize) {
		errno = EMSGSIZE;
		return -1;
	}

	//  Message size must fit within range of size_t data type.
	if (payload_length - 1 > std::numeric_limits<size_t>::max()) {
		errno = EMSGSIZE;
		return -1;
	}

	const size_t msg_size = static_cast<size_t>(payload_length - 1);

	int rc = in_progress.close();
	assert(rc == 0);
	rc = in_progress.init_size(msg_size);
	if (rc != 0) {
		errno_assert(errno == ENOMEM);
		rc = in_progress.init();
		errno_assert(rc == 0);
		errno = ENOMEM;
		return -1;
	}

	next_step(tmpbuf, 1, &v1_decoder_t::flags_ready);
	return 0;
}

int utils::v1_decoder_t::flags_ready(unsigned char const*) {
	//  Store the flags from the wire into the message structure.
	in_progress.set_flags(tmpbuf[0] & msg_t::more);

	next_step(in_progress.data(), in_progress.size(),
			  &v1_decoder_t::message_ready);

	return 0;
}

int utils::v1_decoder_t::message_ready(unsigned char const*) {
	//  Message is completely read. Push it further and start reading
	//  new message. (in_progress is a 0-byte message after this point.)
	next_step(tmpbuf, 1, &v1_decoder_t::one_byte_size_ready);
	return 1;
}
