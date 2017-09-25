/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月25日 星期一 22时47分27秒
 @File Name: msg.hpp
 @Description: from libzmq/src/msg.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_MSG_H_
#define UTILS_ZMQ_MSG_H_

#include <stddef.h>
#include <stdio.h>

#include "atomic_counter/atomic_counter.hpp"
#include "metadata/metadata.hpp"

//  Signature for free function to deallocate the message content.
//  Note that it has to be declared as "C" so that it is the same as
//  zmq_free_fn defined in zmq.h.
extern "C" {
typedef void(msg_free_fn)(void *data, void *hint);
}

namespace utils {

static const int MSG_GROUP_MAX_LENGTH = 15;

//  Some architectures, like sparc64 and some variants of aarch64, enforce
//  pointer alignment and raise sigbus on violations. Make sure applications 
//  allocate utils_msg_t on addresses aligned on a pointer-size boundary to 
//  avoid this issue.
typedef struct utils_msg_t {
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || \
	(defined(__SUNPRO_C) && __SUNPRO_C >= 0x590) ||   \
	(defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x590)
	unsigned char _[64] __attribute__((aligned(sizeof(void *))));
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
	__declspec(align(8)) unsigned char _[64];
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_ARM_ARMV7VE))
	__declspec(align(4)) unsigned char _[64];
#else
	unsigned char _[64];
#endif
} zmq_msg_t;

//  Note that this structure needs to be explicitly constructed
//  (init functions) and destructed (close function).

class msg_t {
public:
	//  Shared message buffer. Message data are either allocated in one
	//  continuous block along with this structure - thus avoiding one
	//  malloc/free pair or they are stored in used-supplied memory.
	//  In the latter case, ffn member stores pointer to the function to be
	//  used to deallocate the data. If the buffer is actually shared (there
	//  are at least 2 references to it) refcount member contains number of
	//  references.
	struct content_t {
		void *data;
		size_t size;
		msg_free_fn *ffn;
		void *hint;
		utils::atomic_counter_t refcnt;
	};

	//  Message flags.
	enum {
		more = 1,	 //  Followed by more parts
		command = 2,  //  Command frame (see ZMTP spec)
		credential = 32,
		identity = 64,
		shared = 128
	};

	bool check();
	int init();

	int init(void *data, size_t size_, msg_free_fn *ffn_, void *hint,
			 content_t *content_ = NULL);

	int init_size(size_t size_);
	int init_data(void *data_, size_t size_, msg_free_fn *ffn_, void *hint_);
	int init_external_storage(content_t *content_, void *data_, size_t size_,
							  msg_free_fn *ffn_, void *hint_);
	int init_delimiter();
	int init_join();
	int init_leave();
	int close();
	int move(msg_t &src_);
	int copy(msg_t &src_);
	void *data();
	size_t size();
	unsigned char flags();
	void set_flags(unsigned char flags_);
	void reset_flags(unsigned char flags_);
	metadata_t *metadata() const;
	void set_metadata(metadata_t *metadata_);
	void reset_metadata();
	bool is_identity() const;
	bool is_credential() const;
	bool is_delimiter() const;
	bool is_join() const;
	bool is_leave() const;
	bool is_vsm() const;
	bool is_cmsg() const;
	bool is_zcmsg() const;
	uint32_t get_routing_id();
	int set_routing_id(uint32_t routing_id_);
	int reset_routing_id();
	const char *group();
	int set_group(const char *group_);
	int set_group(const char *, size_t length);

	//  After calling this function you can copy the message in POD-style
	//  refs_ times. No need to call copy.
	void add_refs(int refs_);

	//  Removes references previously added by add_refs. If the number of
	//  references drops to 0, the message is closed and false is returned.
	bool rm_refs(int refs_);

	//  Size in bytes of the largest message that is still copied around
	//  rather than being reference-counted.
	enum { msg_t_size = 64 };
	enum {
		max_vsm_size =
			msg_t_size - (sizeof(metadata_t *) + 3 + 16 + sizeof(uint32_t))
	};

private:
	utils::atomic_counter_t *refcnt();

	//  Different message types.
	enum type_t {
		type_min = 101,
		//  VSM messages store the content in the message itself
		type_vsm = 101,
		//  LMSG messages store the content in malloc-ed memory
		type_lmsg = 102,
		//  Delimiter messages are used in envelopes
		type_delimiter = 103,
		//  CMSG messages point to constant data
		type_cmsg = 104,

		// zero-copy LMSG message for v2_decoder
		type_zclmsg = 105,

		//  Join message for radio_dish
		type_join = 106,

		//  Leave message for radio_dish
		type_leave = 107,

		type_max = 107
	};

	//  Note that fields shared between different message types are not
	//  moved to the parent class (msg_t). This way we get tighter packing
	//  of the data. Shared fields can be accessed via 'base' member of
	//  the union.
	union {
		struct {
			metadata_t *metadata;
			unsigned char unused[msg_t_size - (sizeof(metadata_t *) + 2 + 16 +
											   sizeof(uint32_t))];
			unsigned char type;
			unsigned char flags;
			char group[16];
			uint32_t routing_id;
		} base;
		struct {
			metadata_t *metadata;
			unsigned char data[max_vsm_size];
			unsigned char size;
			unsigned char type;
			unsigned char flags;
			char group[16];
			uint32_t routing_id;
		} vsm;
		struct {
			metadata_t *metadata;
			content_t *content;
			unsigned char unused[msg_t_size -
								 (sizeof(metadata_t *) + sizeof(content_t *) +
								  2 + 16 + sizeof(uint32_t))];
			unsigned char type;
			unsigned char flags;
			char group[16];
			uint32_t routing_id;
		} lmsg;
		struct {
			metadata_t *metadata;
			content_t *content;
			unsigned char unused[msg_t_size -
								 (sizeof(metadata_t *) + sizeof(content_t *) +
								  2 + 16 + sizeof(uint32_t))];
			unsigned char type;
			unsigned char flags;
			char group[16];
			uint32_t routing_id;
		} zclmsg;
		struct {
			metadata_t *metadata;
			void *data;
			size_t size;
			unsigned char unused[msg_t_size -
								 (sizeof(metadata_t *) + sizeof(void *) +
								  sizeof(size_t) + 2 + 16 + sizeof(uint32_t))];
			unsigned char type;
			unsigned char flags;
			char group[16];
			uint32_t routing_id;
		} cmsg;
		struct {
			metadata_t *metadata;
			unsigned char unused[msg_t_size - (sizeof(metadata_t *) + 2 + 16 +
											   sizeof(uint32_t))];
			unsigned char type;
			unsigned char flags;
			char group[16];
			uint32_t routing_id;
		} delimiter;
	} u;
};
}

#endif
