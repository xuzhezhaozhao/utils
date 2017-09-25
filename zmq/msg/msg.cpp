/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月25日 星期一 22时54分34秒
 @File Name: msg.cpp
 @Description: from libzmq/src/msg.cpp
 ******************************************************/

#include "msg.hpp"
#include "macros/macros.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include "error/err.h"
#include "likely/likely.h"
#include "metadata/metadata.hpp"

//  Check whether the sizes of public representation of the message
//  (utils_msg_t)
//  and private representation of the message (utils::msg_t) match.

typedef char utils_msg_size_check[2 * ((sizeof(utils::msg_t) ==
										sizeof(utils::utils_msg_t)) != 0) -
								  1];

bool utils::msg_t::check() {
	return u.base.type >= type_min && u.base.type <= type_max;
}

int utils::msg_t::init(void *data_, size_t size_, msg_free_fn *ffn_, void *hint,
					   content_t *content_) {
	if (size_ < max_vsm_size) {
		int const rc = init_size(size_);

		if (rc != -1) {
			memcpy(data(), data_, size_);
			return 0;
		} else {
			return -1;
		}
	} else if (content_) {
		return init_external_storage(content_, data_, size_, ffn_, hint);
	} else {
		return init_data(data_, size_, ffn_, hint);
	}
}

int utils::msg_t::init() {
	u.vsm.metadata = NULL;
	u.vsm.type = type_vsm;
	u.vsm.flags = 0;
	u.vsm.size = 0;
	u.vsm.group[0] = '\0';
	u.vsm.routing_id = 0;
	return 0;
}

int utils::msg_t::init_size(size_t size_) {
	if (size_ <= max_vsm_size) {
		u.vsm.metadata = NULL;
		u.vsm.type = type_vsm;
		u.vsm.flags = 0;
		u.vsm.size = (unsigned char)size_;
		u.vsm.group[0] = '\0';
		u.vsm.routing_id = 0;
	} else {
		u.lmsg.metadata = NULL;
		u.lmsg.type = type_lmsg;
		u.lmsg.flags = 0;
		u.lmsg.group[0] = '\0';
		u.lmsg.routing_id = 0;
		u.lmsg.content = NULL;
		if (sizeof(content_t) + size_ > size_)
			u.lmsg.content = (content_t *)malloc(sizeof(content_t) + size_);
		if (unlikely(!u.lmsg.content)) {
			errno = ENOMEM;
			return -1;
		}

		u.lmsg.content->data = u.lmsg.content + 1;
		u.lmsg.content->size = size_;
		u.lmsg.content->ffn = NULL;
		u.lmsg.content->hint = NULL;
		new (&u.lmsg.content->refcnt) utils::atomic_counter_t();
	}
	return 0;
}

int utils::msg_t::init_external_storage(content_t *content_, void *data_,
										size_t size_, msg_free_fn *ffn_,
										void *hint_) {
	utils_assert(NULL != data_);
	utils_assert(NULL != content_);

	u.zclmsg.metadata = NULL;
	u.zclmsg.type = type_zclmsg;
	u.zclmsg.flags = 0;
	u.zclmsg.group[0] = '\0';
	u.zclmsg.routing_id = 0;

	u.zclmsg.content = content_;
	u.zclmsg.content->data = data_;
	u.zclmsg.content->size = size_;
	u.zclmsg.content->ffn = ffn_;
	u.zclmsg.content->hint = hint_;
	new (&u.zclmsg.content->refcnt) utils::atomic_counter_t();

	return 0;
}

int utils::msg_t::init_data(void *data_, size_t size_, msg_free_fn *ffn_,
							void *hint_) {
	//  If data is NULL and size is not 0, a segfault
	//  would occur once the data is accessed
	utils_assert(data_ != NULL || size_ == 0);

	//  Initialize constant message if there's no need to deallocate
	if (ffn_ == NULL) {
		u.cmsg.metadata = NULL;
		u.cmsg.type = type_cmsg;
		u.cmsg.flags = 0;
		u.cmsg.data = data_;
		u.cmsg.size = size_;
		u.cmsg.group[0] = '\0';
		u.cmsg.routing_id = 0;
	} else {
		u.lmsg.metadata = NULL;
		u.lmsg.type = type_lmsg;
		u.lmsg.flags = 0;
		u.lmsg.group[0] = '\0';
		u.lmsg.routing_id = 0;
		u.lmsg.content = (content_t *)malloc(sizeof(content_t));
		if (!u.lmsg.content) {
			errno = ENOMEM;
			return -1;
		}

		u.lmsg.content->data = data_;
		u.lmsg.content->size = size_;
		u.lmsg.content->ffn = ffn_;
		u.lmsg.content->hint = hint_;
		new (&u.lmsg.content->refcnt) utils::atomic_counter_t();
	}
	return 0;
}

int utils::msg_t::init_delimiter() {
	u.delimiter.metadata = NULL;
	u.delimiter.type = type_delimiter;
	u.delimiter.flags = 0;
	u.delimiter.group[0] = '\0';
	u.delimiter.routing_id = 0;
	return 0;
}

int utils::msg_t::init_join() {
	u.base.metadata = NULL;
	u.base.type = type_join;
	u.base.flags = 0;
	u.base.group[0] = '\0';
	u.base.routing_id = 0;
	return 0;
}

int utils::msg_t::init_leave() {
	u.base.metadata = NULL;
	u.base.type = type_leave;
	u.base.flags = 0;
	u.base.group[0] = '\0';
	u.base.routing_id = 0;
	return 0;
}

int utils::msg_t::close() {
	//  Check the validity of the message.
	if (unlikely(!check())) {
		errno = EFAULT;
		return -1;
	}

	if (u.base.type == type_lmsg) {
		//  If the content is not shared, or if it is shared and the reference
		//  count has dropped to zero, deallocate it.
		if (!(u.lmsg.flags & msg_t::shared) || !u.lmsg.content->refcnt.sub(1)) {
			//  We used "placement new" operator to initialize the reference
			//  counter so we call the destructor explicitly now.
			u.lmsg.content->refcnt.~atomic_counter_t();

			if (u.lmsg.content->ffn)
				u.lmsg.content->ffn(u.lmsg.content->data, u.lmsg.content->hint);
			free(u.lmsg.content);
		}
	}

	if (is_zcmsg()) {
		utils_assert(u.zclmsg.content->ffn);

		//  If the content is not shared, or if it is shared and the reference
		//  count has dropped to zero, deallocate it.
		if (!(u.zclmsg.flags & msg_t::shared) ||
			!u.zclmsg.content->refcnt.sub(1)) {
			//  We used "placement new" operator to initialize the reference
			//  counter so we call the destructor explicitly now.
			u.zclmsg.content->refcnt.~atomic_counter_t();

			u.zclmsg.content->ffn(u.zclmsg.content->data,
								  u.zclmsg.content->hint);
		}
	}

	if (u.base.metadata != NULL) {
		if (u.base.metadata->drop_ref()) {
			UTILS_DELETE(u.base.metadata);
		}
		u.base.metadata = NULL;
	}

	//  Make the message invalid.
	u.base.type = 0;

	return 0;
}

int utils::msg_t::move(msg_t &src_) {
	//  Check the validity of the source.
	if (unlikely(!src_.check())) {
		errno = EFAULT;
		return -1;
	}

	int rc = close();
	if (unlikely(rc < 0)) return rc;

	*this = src_;

	rc = src_.init();
	if (unlikely(rc < 0)) return rc;

	return 0;
}

int utils::msg_t::copy(msg_t &src_) {
	//  Check the validity of the source.
	if (unlikely(!src_.check())) {
		errno = EFAULT;
		return -1;
	}

	int rc = close();
	if (unlikely(rc < 0)) return rc;

	if (src_.u.base.type == type_lmsg) {
		//  One reference is added to shared messages. Non-shared messages
		//  are turned into shared messages and reference count is set to 2.
		if (src_.u.lmsg.flags & msg_t::shared)
			src_.u.lmsg.content->refcnt.add(1);
		else {
			src_.u.lmsg.flags |= msg_t::shared;
			src_.u.lmsg.content->refcnt.set(2);
		}
	}

	if (src_.is_zcmsg()) {
		//  One reference is added to shared messages. Non-shared messages
		//  are turned into shared messages and reference count is set to 2.
		if (src_.u.zclmsg.flags & msg_t::shared)
			src_.refcnt()->add(1);
		else {
			src_.u.zclmsg.flags |= msg_t::shared;
			src_.refcnt()->set(2);
		}
	}
	if (src_.u.base.metadata != NULL) src_.u.base.metadata->add_ref();

	*this = src_;

	return 0;
}

void *utils::msg_t::data() {
	//  Check the validity of the message.
	utils_assert(check());

	switch (u.base.type) {
		case type_vsm:
			return u.vsm.data;
		case type_lmsg:
			return u.lmsg.content->data;
		case type_cmsg:
			return u.cmsg.data;
		case type_zclmsg:
			return u.zclmsg.content->data;
		default:
			utils_assert(false);
			return NULL;
	}
}

size_t utils::msg_t::size() {
	//  Check the validity of the message.
	utils_assert(check());

	switch (u.base.type) {
		case type_vsm:
			return u.vsm.size;
		case type_lmsg:
			return u.lmsg.content->size;
		case type_zclmsg:
			return u.zclmsg.content->size;
		case type_cmsg:
			return u.cmsg.size;
		default:
			utils_assert(false);
			return 0;
	}
}

unsigned char utils::msg_t::flags() { return u.base.flags; }

void utils::msg_t::set_flags(unsigned char flags_) { u.base.flags |= flags_; }

void utils::msg_t::reset_flags(unsigned char flags_) {
	u.base.flags &= ~flags_;
}

utils::metadata_t *utils::msg_t::metadata() const { return u.base.metadata; }

void utils::msg_t::set_metadata(utils::metadata_t *metadata_) {
	assert(metadata_ != NULL);
	assert(u.base.metadata == NULL);
	metadata_->add_ref();
	u.base.metadata = metadata_;
}

void utils::msg_t::reset_metadata() {
	if (u.base.metadata) {
		if (u.base.metadata->drop_ref()) {
			UTILS_DELETE(u.base.metadata);
		}
		u.base.metadata = NULL;
	}
}

bool utils::msg_t::is_identity() const {
	return (u.base.flags & identity) == identity;
}

bool utils::msg_t::is_credential() const {
	return (u.base.flags & credential) == credential;
}

bool utils::msg_t::is_delimiter() const {
	return u.base.type == type_delimiter;
}

bool utils::msg_t::is_vsm() const { return u.base.type == type_vsm; }

bool utils::msg_t::is_cmsg() const { return u.base.type == type_cmsg; }

bool utils::msg_t::is_zcmsg() const { return u.base.type == type_zclmsg; }

bool utils::msg_t::is_join() const { return u.base.type == type_join; }

bool utils::msg_t::is_leave() const { return u.base.type == type_leave; }

void utils::msg_t::add_refs(int refs_) {
	utils_assert(refs_ >= 0);

	//  Operation not supported for messages with metadata.
	utils_assert(u.base.metadata == NULL);

	//  No copies required.
	if (!refs_) return;

	//  VSMs, CMSGS and delimiters can be copied straight away. The only
	//  message type that needs special care are long messages.
	if (u.base.type == type_lmsg || is_zcmsg()) {
		if (u.base.flags & msg_t::shared)
			refcnt()->add(refs_);
		else {
			refcnt()->set(refs_ + 1);
			u.base.flags |= msg_t::shared;
		}
	}
}

bool utils::msg_t::rm_refs(int refs_) {
	utils_assert(refs_ >= 0);

	//  Operation not supported for messages with metadata.
	utils_assert(u.base.metadata == NULL);

	//  No copies required.
	if (!refs_) return true;

	//  If there's only one reference close the message.
	if ((u.base.type != type_zclmsg && u.base.type != type_lmsg) ||
		!(u.base.flags & msg_t::shared)) {
		close();
		return false;
	}

	//  The only message type that needs special care are long and zcopy
	//  messages.
	if (u.base.type == type_lmsg && !u.lmsg.content->refcnt.sub(refs_)) {
		//  We used "placement new" operator to initialize the reference
		//  counter so we call the destructor explicitly now.
		u.lmsg.content->refcnt.~atomic_counter_t();

		if (u.lmsg.content->ffn)
			u.lmsg.content->ffn(u.lmsg.content->data, u.lmsg.content->hint);
		free(u.lmsg.content);

		return false;
	}

	if (is_zcmsg() && !u.zclmsg.content->refcnt.sub(refs_)) {
		// storage for rfcnt is provided externally
		if (u.zclmsg.content->ffn) {
			u.zclmsg.content->ffn(u.zclmsg.content->data,
								  u.zclmsg.content->hint);
		}

		return false;
	}

	return true;
}

uint32_t utils::msg_t::get_routing_id() { return u.base.routing_id; }

int utils::msg_t::set_routing_id(uint32_t routing_id_) {
	if (routing_id_) {
		u.base.routing_id = routing_id_;
		return 0;
	}
	errno = EINVAL;
	return -1;
}

int utils::msg_t::reset_routing_id() {
	u.base.routing_id = 0;
	return 0;
}

const char *utils::msg_t::group() { return u.base.group; }

int utils::msg_t::set_group(const char *group_) {
	return set_group(group_, strlen(group_));
}

int utils::msg_t::set_group(const char *group_, size_t length_) {
	if (length_ > MSG_GROUP_MAX_LENGTH) {
		errno = EINVAL;
		return -1;
	}

	strncpy(u.base.group, group_, length_);
	u.base.group[length_] = '\0';

	return 0;
}

utils::atomic_counter_t *utils::msg_t::refcnt() {
	switch (u.base.type) {
		case type_lmsg:
			return &u.lmsg.content->refcnt;
		case type_zclmsg:
			return &u.zclmsg.content->refcnt;
		default:
			utils_assert(false);
			return NULL;
	}
}
