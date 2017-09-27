/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月26日 星期二 22时10分25秒
 @File Name: decoder_allocators.cpp
 @Description: from libzmq/src/decoder_allocators.cpp
 ******************************************************/

#include "decoder_allocators.hpp"
#include "msg/msg.hpp"

#include <cmath> // for ceil()

utils::shared_message_memory_allocator::shared_message_memory_allocator(
	std::size_t bufsize_)
	: buf(NULL),
	  bufsize(0),
	  max_size(bufsize_),
	  msg_content(NULL),
	  maxCounters(static_cast<size_t>(
		  std::ceil(static_cast<double>(max_size) /
					static_cast<double>(msg_t::max_vsm_size)))) {}

utils::shared_message_memory_allocator::shared_message_memory_allocator(
	std::size_t bufsize_, std::size_t maxMessages)
	: buf(NULL),
	  bufsize(0),
	  max_size(bufsize_),
	  msg_content(NULL),
	  maxCounters(maxMessages) {}

utils::shared_message_memory_allocator::~shared_message_memory_allocator() {
	deallocate();
}

//  分配的内存布局是 [atomic_counter_t] + max_counter*[content_t] + max_size
unsigned char* utils::shared_message_memory_allocator::allocate() {
	if (buf) {
		// release reference count to couple lifetime to messages
		utils::atomic_counter_t* c =
			reinterpret_cast<utils::atomic_counter_t*>(buf);

		// if refcnt drops to 0, there are no message using the buffer
		// because either all messages have been closed or only vsm-messages
		// were created
		if (c->sub(1)) {
			// buffer is still in use as message data. "Release" it and create a
			// new one
			// release pointer because we are going to create a new buffer
			release();
		}
	}

	// if buf != NULL it is not used by any message so we can re-use it for the
	// next run
	if (!buf) {
		// allocate memory for reference counters together with reception buffer
		std::size_t const allocationsize =
			max_size + sizeof(utils::atomic_counter_t) +
			maxCounters * sizeof(utils::msg_t::content_t);

		buf = static_cast<unsigned char*>(std::malloc(allocationsize));
		alloc_assert(buf);

		new (buf) atomic_counter_t(1);
	} else {
		// release reference count to couple lifetime to messages
		utils::atomic_counter_t* c =
			reinterpret_cast<utils::atomic_counter_t*>(buf);
		c->set(1);
	}

	bufsize = max_size;
	msg_content = reinterpret_cast<utils::msg_t::content_t*>(
		buf + sizeof(atomic_counter_t) + max_size);
	return buf + sizeof(utils::atomic_counter_t);
}

void utils::shared_message_memory_allocator::deallocate() {
	utils::atomic_counter_t* c =
		reinterpret_cast<utils::atomic_counter_t*>(buf);
	if (buf && !c->sub(1)) {
		std::free(buf);
	}
	release();
}

unsigned char* utils::shared_message_memory_allocator::release() {
	unsigned char* b = buf;
	buf = NULL;
	bufsize = 0;
	msg_content = NULL;

	return b;
}

void utils::shared_message_memory_allocator::inc_ref() {
	(reinterpret_cast<utils::atomic_counter_t*>(buf))->add(1);
}

void utils::shared_message_memory_allocator::call_dec_ref(void*, void* hint) {
	utils_assert(hint);
	unsigned char* buf = static_cast<unsigned char*>(hint);
	utils::atomic_counter_t* c =
		reinterpret_cast<utils::atomic_counter_t*>(buf);

	if (!c->sub(1)) {
		c->~atomic_counter_t();
		std::free(buf);
		buf = NULL;
	}
}

std::size_t utils::shared_message_memory_allocator::size() const {
	return bufsize;
}

unsigned char* utils::shared_message_memory_allocator::data() {
	return buf + sizeof(utils::atomic_counter_t);
}
