/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月25日 星期一 22时51分50秒
 @File Name: metadata.cpp
 @Description: from libzmq/src/metadata.cpp
 ******************************************************/

#include "metadata.hpp"

utils::metadata_t::metadata_t(const dict_t &dict) : ref_cnt(1), dict(dict) {}

const char *utils::metadata_t::get(const std::string &property) const {
	dict_t::const_iterator it = dict.find(property);
	if (it == dict.end())
		return NULL;
	else
		return it->second.c_str();
}

void utils::metadata_t::add_ref() { ref_cnt.add(1); }

bool utils::metadata_t::drop_ref() { return !ref_cnt.sub(1); }
