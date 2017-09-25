/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月25日 星期一 22时50分35秒
 @File Name: metadata.hpp
 @Description: from libzmq/src/metadata.hpp
 ******************************************************/

#ifndef UTILS_ZMQ_METADATA_H_
#define UTILS_ZMQ_METADATA_H_

#include "atomic_counter/atomic_counter.hpp"

#include <map>
#include <string>

namespace utils {
class metadata_t {
public:
	typedef std::map<std::string, std::string> dict_t;

	metadata_t(const dict_t &dict);

	//  Returns pointer to property value or NULL if
	//  property is not found.
	const char *get(const std::string &property) const;

	void add_ref();

	//  Drop reference. Returns true iff the reference
	//  counter drops to zero.
	bool drop_ref();

private:
	metadata_t(const metadata_t &);
	metadata_t &operator=(const metadata_t &);

	//  Reference counter.
	atomic_counter_t ref_cnt;

	//  Dictionary holding metadata.
	dict_t dict;
};
}

#endif
