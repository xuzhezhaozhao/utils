/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 31 Oct 2017 11:43:00 AM CST
 @File Name: buffer.cpp
 @Description:
 ******************************************************/

#include "buffer.h"

#include <string.h>
#include <new>

namespace utils {

Buffer::Buffer() : data_(initdata_), size_(DEFAULT_BUFFERSIZE), used_(0) {}

Buffer::~Buffer() {
	if (!isbuffonstack()) {
		// free heap buffer
		delete[] data_;
	}
}

bool Buffer::addstring(const char* s, size_t len) {
	char* inpos = prepbuffsize(len);
	if (inpos == NULL) {
		return false;
	}
	memcpy(inpos, s, len * sizeof(char));
	used_ += len;

	return true;
}

void Buffer::clear() {
	if (!isbuffonstack()) {
		// free heap buffer
		delete[] data_;
	}
	data_ = initdata_;
	size_ = DEFAULT_BUFFERSIZE;
	used_ = 0;
}

char* Buffer::prepbuffsize(size_t sz) {
	if (size_ - used_ < sz) {		 // not enough space?
		size_t newsize = size_ * 2;  // double buffer size
		if (newsize - used_ < sz) {  // not big enough?
			newsize = used_ + sz;
		}
		if (newsize < used_ || newsize - used_ < sz) {  // overflow?
			// buffer too large
			return NULL;
		}

		char* newbuff = new (std::nothrow) char[newsize];
		if (newbuff == NULL) {
			return NULL;
		}
		// copy original content
		memcpy(newbuff, data_, used_ * sizeof(char));

		if (!isbuffonstack()) {
			// free original heap buffer
			delete[] data_;
		}

		data_ = newbuff;
		size_ = newsize;
	}

	return &data_[used_];
}

}
