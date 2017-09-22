/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 20时56分40秒
 @File Name: ypipe_base.hpp
 @Description: from libzmq/src/ypipe_base.hpp
 ******************************************************/

#ifndef UTILS_YPIPE_BASE_H_
#define UTILS_YPIPE_BASE_H_

namespace utils {

// ypipe_base abstracts ypipe and ypipe_conflate specific
// classes, one is selected according to a the conflate
// socket option

template <typename T>
class ypipe_base_t {
public:
	virtual ~ypipe_base_t() {}
	virtual void write(const T &value_, bool incomplete_) = 0;
	virtual bool unwrite(T *value_) = 0;
	virtual bool flush() = 0;
	virtual bool check_read() = 0;
	virtual bool read(T *value_) = 0;
	virtual bool probe(bool (*fn)(const T &)) = 0;
};
}

#endif
