/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 21时25分04秒
 @File Name: atomic_ptr.hpp
 @Description: from libzmq/src/atomic_ptr.hpp
 ******************************************************/

#ifndef UTILS_ATOMIC_PTR_H_
#define UTILS_ATOMIC_PTR_H_

// 保留以下兼容性的宏, 手动为本机指定版本
#define UTILS_CXX11 1
#define UTILS_HAVE_ATOMIC_INTRINSICS 1

#if defined UTILS_FORCE_MUTEXES
#define UTILS_ATOMIC_PTR_MUTEX
#elif defined UTILS_HAVE_ATOMIC_INTRINSICS
#define UTILS_ATOMIC_PTR_INTRINSIC
#elif (defined UTILS_CXX11 && defined __cplusplus && __cplusplus >= 201103L)
#define UTILS_ATOMIC_PTR_CXX11
#elif (defined __i386__ || defined __x86_64__) && defined __GNUC__
#define UTILS_ATOMIC_PTR_X86
#elif defined __ARM_ARCH_7A__ && defined __GNUC__
#define UTILS_ATOMIC_PTR_ARM
#elif defined __tile__
#define UTILS_ATOMIC_PTR_TILE
#elif defined UTILS_HAVE_WINDOWS
#define UTILS_ATOMIC_PTR_WINDOWS
#elif (defined UTILS_HAVE_SOLARIS || defined UTILS_HAVE_NETBSD || \
	   defined UTILS_HAVE_GNU)
#define UTILS_ATOMIC_PTR_ATOMIC_H
#else
#define UTILS_ATOMIC_PTR_MUTEX
#endif

#if defined UTILS_ATOMIC_PTR_MUTEX
#include "mutex/mutex.hpp"
#elif defined UTILS_ATOMIC_PTR_CXX11
#include <atomic>
#elif defined UTILS_ATOMIC_PTR_WINDOWS
#include "windows.hpp"
#elif defined UTILS_ATOMIC_PTR_ATOMIC_H
#include <atomic.h>
#elif defined UTILS_ATOMIC_PTR_TILE
#include <arch/atomic.h>
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
#if defined UTILS_ATOMIC_PTR_WINDOWS
		return (T *)InterlockedExchangePointer((PVOID *)&ptr, val_);
#elif defined UTILS_ATOMIC_PTR_INTRINSIC
		return (T *)__atomic_exchange_n(&ptr, val_, __ATOMIC_ACQ_REL);
#elif defined UTILS_ATOMIC_PTR_CXX11
		return ptr.exchange(val_, std::memory_order_acq_rel);
#elif defined UTILS_ATOMIC_PTR_ATOMIC_H
		return (T *)atomic_swap_ptr(&ptr, val_);
#elif defined UTILS_ATOMIC_PTR_TILE
		return (T *)arch_atomic_exchange(&ptr, val_);
#elif defined UTILS_ATOMIC_PTR_X86
		T *old;
		__asm__ volatile("lock; xchg %0, %2"
						 : "=r"(old), "=m"(ptr)
						 : "m"(ptr), "0"(val_));
		return old;
#elif defined UTILS_ATOMIC_PTR_ARM
		T *old;
		unsigned int flag;
		__asm__ volatile(
			"       dmb     sy\n\t"
			"1:     ldrex   %1, [%3]\n\t"
			"       strex   %0, %4, [%3]\n\t"
			"       teq     %0, #0\n\t"
			"       bne     1b\n\t"
			"       dmb     sy\n\t"
			: "=&r"(flag), "=&r"(old), "+Qo"(ptr)
			: "r"(&ptr), "r"(val_)
			: "cc");
		return old;
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
#if defined UTILS_ATOMIC_PTR_WINDOWS
		return (T *)InterlockedCompareExchangePointer((volatile PVOID *)&ptr,
													  val_, cmp_);
#elif defined UTILS_ATOMIC_PTR_INTRINSIC
		T *old = cmp_;
		__atomic_compare_exchange_n(&ptr, (volatile T **)&old, val_, false,
									__ATOMIC_RELEASE, __ATOMIC_ACQUIRE);
		return old;
#elif defined UTILS_ATOMIC_PTR_CXX11
		ptr.compare_exchange_strong(cmp_, val_, std::memory_order_acq_rel);
		return cmp_;
#elif defined UTILS_ATOMIC_PTR_ATOMIC_H
		return (T *)atomic_cas_ptr(&ptr, cmp_, val_);
#elif defined UTILS_ATOMIC_PTR_TILE
		return (T *)arch_atomic_val_compare_and_exchange(&ptr, cmp_, val_);
#elif defined UTILS_ATOMIC_PTR_X86
		T *old;
		__asm__ volatile("lock; cmpxchg %2, %3"
						 : "=a"(old), "=m"(ptr)
						 : "r"(val_), "m"(ptr), "0"(cmp_)
						 : "cc");
		return old;
#elif defined UTILS_ATOMIC_PTR_ARM
		T *old;
		unsigned int flag;
		__asm__ volatile(
			"       dmb     sy\n\t"
			"1:     ldrex   %1, [%3]\n\t"
			"       mov     %0, #0\n\t"
			"       teq     %1, %4\n\t"
			"       it      eq\n\t"
			"       strexeq %0, %5, [%3]\n\t"
			"       teq     %0, #0\n\t"
			"       bne     1b\n\t"
			"       dmb     sy\n\t"
			: "=&r"(flag), "=&r"(old), "+Qo"(ptr)
			: "r"(&ptr), "r"(cmp_), "r"(val_)
			: "cc");
		return old;
#elif defined UTILS_ATOMIC_PTR_MUTEX
		sync.lock();
		T *old = (T *)ptr;
		if (ptr == cmp_) ptr = val_;
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

//  Remove macros local to this file.
#undef UTILS_ATOMIC_PTR_MUTEX
#undef UTILS_ATOMIC_PTR_INTRINSIC
#undef UTILS_ATOMIC_PTR_CXX11
#undef UTILS_ATOMIC_PTR_X86
#undef UTILS_ATOMIC_PTR_ARM
#undef UTILS_ATOMIC_PTR_TILE
#undef UTILS_ATOMIC_PTR_WINDOWS
#undef UTILS_ATOMIC_PTR_ATOMIC_H

#endif
