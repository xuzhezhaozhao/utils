/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Wed 27 Sep 2017 01:55:48 PM CST
 @File Name: decoder.hpp
 @Description: from libzmq/src/decoder.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_DECODER_HPP_
#define UTILS_ZMQ_DECODER_HPP_

#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <cstring>

#include "decoder_allocators/decoder_allocators.hpp"
#include "error/err.h"
#include "i_decoder.hpp"
#include "msg/msg.hpp"

namespace utils {
//  Helper base class for decoders that know the amount of data to read
//  in advance at any moment. Knowing the amount in advance is a property
//  of the protocol used. 0MQ framing protocol is based size-prefixed
//  paradigm, which qualifies it to be parsed by this class.
//  On the other hand, XML-based transports (like XMPP or SOAP) don't allow
//  for knowing the size of data to read in advance and should use different
//  decoding algorithms.
//
//  This class implements the state machine that parses the incoming buffer.
//  Derived class should implement individual state machine actions.
//
//  Buffer management is done by an allocator policy.
template <typename T, typename A = c_single_allocator>
class decoder_base_t : public i_decoder {
public:
	explicit decoder_base_t(A *allocator_)
		: next(NULL), read_pos(NULL), to_read(0), allocator(allocator_) {
		buf = allocator->allocate();
	}

	//  The destructor doesn't have to be virtual. It is made virtual
	//  just to keep ICC and code checking tools from complaining.
	virtual ~decoder_base_t() { allocator->deallocate(); }

	//  Returns a buffer to be filled with binary data.
	void get_buffer(unsigned char **data_, std::size_t *size_) {
		buf = allocator->allocate();

		//  If we are expected to read large message, we'll opt for zero-
		//  copy, i.e. we'll ask caller to fill the data directly to the
		//  message. Note that subsequent read(s) are non-blocking, thus
		//  each single read reads at most SO_RCVBUF bytes at once not
		//  depending on how large is the chunk returned from here.
		//  As a consequence, large messages being received won't block
		//  other engines running in the same I/O thread for excessive
		//  amounts of time.
		if (to_read >= allocator->size()) {
			*data_ = read_pos;
			*size_ = to_read;
			return;
		}

		*data_ = buf;
		*size_ = allocator->size();
	}

	//  Processes the data in the buffer previously allocated using
	//  get_buffer function. size_ argument specifies number of bytes
	//  actually filled into the buffer. Function returns 1 when the
	//  whole message was decoded or 0 when more data is required.
	//  On error, -1 is returned and errno set accordingly.
	//  Number of bytes processed is returned in bytes_used_.
	int decode(const unsigned char *data_, std::size_t size_,
			   std::size_t &bytes_used_) {
		bytes_used_ = 0;

		//  In case of zero-copy simply adjust the pointers, no copying
		//  is required. Also, run the state machine in case all the data
		//  were processed.
		if (data_ == read_pos) {
			zmq_assert(size_ <= to_read);
			read_pos += size_;
			to_read -= size_;
			bytes_used_ = size_;

			while (!to_read) {
				const int rc =
					(static_cast<T *>(this)->*next)(data_ + bytes_used_);
				if (rc != 0) return rc;
			}
			return 0;
		}

		while (bytes_used_ < size_) {
			//  Copy the data from buffer to the message.
			const size_t to_copy = std::min(to_read, size_ - bytes_used_);
			// Only copy when destination address is different from the
			// current address in the buffer.
			if (read_pos != data_ + bytes_used_) {
				memcpy(read_pos, data_ + bytes_used_, to_copy);
			}

			read_pos += to_copy;
			to_read -= to_copy;
			bytes_used_ += to_copy;
			//  Try to get more space in the message to fill in.
			//  If none is available, return.
			while (to_read == 0) {
				// pass current address in the buffer
				const int rc =
					(static_cast<T *>(this)->*next)(data_ + bytes_used_);
				if (rc != 0) return rc;
			}
		}

		return 0;
	}

	virtual void resize_buffer(std::size_t new_size) {
		allocator->resize(new_size);
	}

protected:
	//  Prototype of state machine action. Action should return false if
	//  it is unable to push the data to the system.
	typedef int (T::*step_t)(unsigned char const *);

	//  This function should be called from derived class to read data
	//  from the buffer and schedule next state machine action.
	void next_step(void *read_pos_, std::size_t to_read_, step_t next_) {
		read_pos = static_cast<unsigned char *>(read_pos_);
		to_read = to_read_;
		next = next_;
	}

private:
	//  Next step. If set to NULL, it means that associated data stream
	//  is dead. Note that there can be still data in the process in such
	//  case.
	step_t next;

	//  Where to store the read data.
	unsigned char *read_pos;

	//  How much data to read before taking next step.
	std::size_t to_read;

	//  The duffer for data to decode.
	A *allocator;
	unsigned char *buf;

	decoder_base_t(const decoder_base_t &);
	const decoder_base_t &operator=(const decoder_base_t &);
};
}

#endif
