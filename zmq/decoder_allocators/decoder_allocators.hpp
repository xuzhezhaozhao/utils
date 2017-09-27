/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月26日 星期二 22时02分42秒
 @File Name: decoder_allocators.hpp
 @Description: from libzmq/src/decoder_allocators.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_DECODER_ALLOCATORS_HPP_
#define UTILS_ZMQ_DECODER_ALLOCATORS_HPP_

#include <cstddef>
#include <cstdlib>

#include "atomic_counter/atomic_counter.hpp"
#include "error/err.h"
#include "msg/msg.hpp"

namespace utils {
// Static buffer policy.
class c_single_allocator {
public:
	explicit c_single_allocator(std::size_t bufsize_)
		: bufsize(bufsize_),
		  buf(static_cast<unsigned char*>(std::malloc(bufsize))) {
		alloc_assert(buf);
	}

	~c_single_allocator() { std::free(buf); }

	unsigned char* allocate() { return buf; }

	void deallocate() {}

	std::size_t size() const { return bufsize; }

	void resize(std::size_t new_size) { bufsize = new_size; }

private:
	std::size_t bufsize;
	unsigned char* buf;

	c_single_allocator(c_single_allocator const&);
	c_single_allocator& operator=(c_single_allocator const&);
};

// This allocator allocates a reference counted buffer which is used by
// v2_decoder_t to use zero-copy msg::init_data to create messages with memory
// from this buffer as data storage.
//
// The buffer is allocated with a reference count of 1 to make sure that is is
// alive while decoding messages. Otherwise, it is possible that e.g. the first 
// message increases the count from zero to one, gets passed to the user 
// application, processed in the user thread and deleted which would then 
// deallocate the buffer. The drawback is that the buffer may be allocated 
// longer than necessary because it is only deleted when allocate is called the
// next time.
class shared_message_memory_allocator {
public:
	explicit shared_message_memory_allocator(std::size_t bufsize_);

	// Create an allocator for a maximum number of messages
	shared_message_memory_allocator(std::size_t bufsize_,
									std::size_t maxMessages);

	~shared_message_memory_allocator();

	// Allocate a new buffer
	//
	// This releases the current buffer to be bound to the lifetime of the
	// messages
	// created on this buffer.
	unsigned char* allocate();

	// force deallocation of buffer.
	void deallocate();

	// Give up ownership of the buffer. The buffer's lifetime is now coupled to
	// the messages constructed on top of it.
	unsigned char* release();

	void inc_ref();

	static void call_dec_ref(void*, void* buffer);

	std::size_t size() const;

	// Return pointer to the first message data byte.
	unsigned char* data();

	// Return pointer to the first byte of the buffer.
	unsigned char* buffer() { return buf; }

	// 调用 allocate 才会真正分配内存
	void resize(std::size_t new_size) { bufsize = new_size; }

	utils::msg_t::content_t* provide_content() { return msg_content; }

	void advance_content() { msg_content++; }

private:
	//  分配的内存布局是 [atomic_counter_t] + max_counter*[content_t] + max_size
	//  最开头的 atomic_counter_t 是该内存块的引用计数器
	//  buf 指向起始位置
	//  msg_content 指向第一个 content_t
	//  max_size 为缓冲区最大大小
	unsigned char* buf;
	std::size_t bufsize;
	std::size_t max_size;
	utils::msg_t::content_t* msg_content;
	std::size_t maxCounters;
};
}

#endif
