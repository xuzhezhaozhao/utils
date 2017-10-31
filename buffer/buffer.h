/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 31 Oct 2017 11:39:13 AM CST
 @File Name: buffer.h
 @Description: inspired by lua-5.3.4/src/lauxlib.c (luaL_Buffer)
 ******************************************************/

#ifndef UTILS_BUFFER_H_
#define UTILS_BUFFER_H_

#include <stdlib.h>

namespace utils {

class Buffer {
public:
	static const int DEFAULT_BUFFERSIZE = 8192;

	Buffer();
	~Buffer();

	// add string s to buffer
	bool addstring(const char* s, size_t sz);

	// clear buffer, reset buffer to init state
	void clear();

	size_t size() const { return size_; }
	size_t used() const { return used_; }
	char* data() const { return data_; }

private:
	// check whether buffer use inner storage as a buffer
	bool isbuffonstack() const { return data_ == initdata_; }

	// returns a pointer to a free area with at least 'sz' bytes
	// returns NULL if the function needs allocate storage and fails, and
	// current buffer is still valid
	char* prepbuffsize(size_t sz);

	char* data_;						// buffer address
	size_t size_;						// size of allocated storage
	size_t used_;						// number of characters in buffer
	char initdata_[DEFAULT_BUFFERSIZE];  // initial buffer

	// non-copyable
	Buffer(const Buffer&);
	const Buffer& operator=(const Buffer&);
};
}

#endif
