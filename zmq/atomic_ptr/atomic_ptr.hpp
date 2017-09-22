/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 21时25分04秒
 @File Name: atomic_ptr.hpp
 @Description: from libzmq/src/atomic_ptr.hpp
 ******************************************************/

#ifndef UTILS_ATOMIC_PTR_H_
#define UTILS_ATOMIC_PTR_H_

#if defined __cplusplus && __cplusplus >= 201103L
#define UTILS_ATOMIC_PTR_CXX11
#else
#define UTILS_ATOMIC_PTR_MUTEX
#endif

#if defined UTILS_ATOMIC_PTR_CXX11
#include <atomic>
#elif defined UTILS_ATOMIC_PTR_MUTEX
#include "mutex/mutex.hpp"
#endif

#include <stdio.h>  // for NULL

namespace utils {

//  This class encapsulates several atomic operations on pointers.

template <typename T>
class atomic_ptr_t {
public:
	//  Initialise atomic pointer
	inline atomic_ptr_t() { ptr = NULL; }

	//  Destroy atomic pointer
	inline ~atomic_ptr_t() {}

	//  Set value of atomic pointer in a non-threadsafe way
	//  Use this function only when you are sure that at most one
	//  thread is accessing the pointer at the moment.
	inline void set(T *ptr_) { this->ptr = ptr_; }

	//  Perform atomic 'exchange pointers' operation. Pointer is set
	//  to the 'val' value. Old value is returned.
	inline T *xchg(T *val_) {
#if defined UTILS_ATOMIC_PTR_CXX11
		return ptr.exchange(val_, std::memory_order_acq_rel);
#elif defined UTILS_ATOMIC_PTR_MUTEX
		sync.lock();
		T *old = (T *)ptr;
		ptr = val_;
		sync.unlock();
		return old;
#else
#error atomic_ptr is not implemented for this platform
#endif
	}

	//  Perform atomic 'compare and swap' operation on the pointer.
	//  The pointer is compared to 'cmp' argument and if they are
	//  equal, its value is set to 'val'. Old value of the pointer
	//  is returned.
	inline T *cas(T *cmp_, T *val_) {
#if defined UTILS_ATOMIC_PTR_CXX11
		ptr.compare_exchange_strong(cmp_, val_, std::memory_order_acq_rel);
		return cmp_;
#elif defined UTILS_ATOMIC_PTR_MUTEX
		sync.lock();
		T *old = (T *)ptr;
		if (ptr == cmp_) {
			ptr = val_;
		}
		sync.unlock();
		return old;
#else
#error atomic_ptr is not implemented for this platform
#endif
	}

private:
#if defined UTILS_ATOMIC_PTR_CXX11
	std::atomic<T *> ptr;
#else
	volatile T *ptr;
#endif

#if defined UTILS_ATOMIC_PTR_MUTEX
	mutex_t sync;
#endif

#if !defined UTILS_ATOMIC_PTR_CXX11
	atomic_ptr_t(const atomic_ptr_t &);
	const atomic_ptr_t &operator=(const atomic_ptr_t &);
#endif
};
}

#endif
