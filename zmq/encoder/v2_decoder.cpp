/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 02:24:18 PM CST
 @File Name: v2_decoder.cpp
 @Description: from libzmq/src/v2_decoder.cpp
 ******************************************************/

#include <stdlib.h>
#include <string.h>
#include <cmath>

#include "error/err.h"
#include "likely/likely.h"
#include "v2_decoder.hpp"
#include "v2_protocol.hpp"
#include "wire/wire.hpp"

utils::v2_decoder_t::v2_decoder_t(size_t bufsize_, int64_t maxmsgsize_)
	: shared_message_memory_allocator(bufsize_),
	  decoder_base_t<v2_decoder_t, shared_message_memory_allocator>(this),
	  msg_flags(0),
	  maxmsgsize(maxmsgsize_) {
	int rc = in_progress.init();
	errno_assert(rc == 0);

	//  At the beginning, read one byte and go to flags_ready state.
	next_step(tmpbuf, 1, &v2_decoder_t::flags_ready);
}

utils::v2_decoder_t::~v2_decoder_t() {
	int rc = in_progress.close();
	errno_assert(rc == 0);
}

int utils::v2_decoder_t::flags_ready(unsigned char const*) {
	msg_flags = 0;
	if (tmpbuf[0] & v2_protocol_t::more_flag) msg_flags |= msg_t::more;
	if (tmpbuf[0] & v2_protocol_t::command_flag) msg_flags |= msg_t::command;

	//  The payload length is either one or eight bytes,
	//  depending on whether the 'large' bit is set.
	if (tmpbuf[0] & v2_protocol_t::large_flag)
		next_step(tmpbuf, 8, &v2_decoder_t::eight_byte_size_ready);
	else
		next_step(tmpbuf, 1, &v2_decoder_t::one_byte_size_ready);

	return 0;
}

int utils::v2_decoder_t::one_byte_size_ready(unsigned char const* read_from) {
	return size_ready(tmpbuf[0], read_from);
}

int utils::v2_decoder_t::eight_byte_size_ready(unsigned char const* read_from) {
	//  The payload size is encoded as 64-bit unsigned integer.
	//  The most significant byte comes first.
	const uint64_t msg_size = get_uint64(tmpbuf);

	return size_ready(msg_size, read_from);
}

int utils::v2_decoder_t::size_ready(uint64_t msg_size,
									unsigned char const* read_pos) {
	//  Message size must not exceed the maximum allowed size.
	if (maxmsgsize >= 0)
		if (unlikely(msg_size > static_cast<uint64_t>(maxmsgsize))) {
			errno = EMSGSIZE;
			return -1;
		}

	//  Message size must fit into size_t data type.
	if (unlikely(msg_size != static_cast<size_t>(msg_size))) {
		errno = EMSGSIZE;
		return -1;
	}

	int rc = in_progress.close();
	assert(rc == 0);

	// the current message can exceed the current buffer. We have to copy the
	// buffer
	// data into a new message and complete it in the next receive.

	if (unlikely((unsigned char*)read_pos + msg_size > (data() + size()))) {
		// a new message has started, but the size would exceed the
		// pre-allocated arena
		// this happens every time when a message does not fit completely into
		// the buffer
		rc = in_progress.init_size(static_cast<size_t>(msg_size));
	} else {
		// construct message using n bytes from the buffer as storage
		// increase buffer ref count
		// if the message will be a large message, pass a valid refcnt memory
		// location as well
		rc = in_progress.init((unsigned char*)read_pos,
							  static_cast<size_t>(msg_size),
							  shared_message_memory_allocator::call_dec_ref,
							  buffer(), provide_content());

		// For small messages, data has been copied and refcount does not have
		// to be increased
		if (in_progress.is_zcmsg()) {
			advance_content();
			inc_ref();
		}
	}

	if (unlikely(rc)) {
		errno_assert(errno == ENOMEM);
		rc = in_progress.init();
		errno_assert(rc == 0);
		errno = ENOMEM;
		return -1;
	}

	in_progress.set_flags(msg_flags);
	// this sets read_pos to
	// the message data address if the data needs to be copied
	// for small message / messages exceeding the current buffer
	// or
	// to the current start address in the buffer because the message
	// was constructed to use n bytes from the address passed as argument
	next_step(in_progress.data(), in_progress.size(),
			  &v2_decoder_t::message_ready);

	return 0;
}

int utils::v2_decoder_t::message_ready(unsigned char const*) {
	//  Message is completely read. Signal this to the caller
	//  and prepare to decode next message.
	next_step(tmpbuf, 1, &v2_decoder_t::flags_ready);
	return 1;
}
