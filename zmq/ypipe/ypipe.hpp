/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月22日 星期五 21时02分45秒
 @File Name: ypipe.hpp
 @Description: from libzmq/src/ypipe.hpp
 ******************************************************/

#ifndef UTILS_YPIPE_H_
#define UTILS_YPIPE_H_

#include "atomic_ptr/atomic_ptr.hpp"
#include "ypipe_base.hpp"
#include "yqueue/yqueue.hpp"

namespace utils {

//  Lock-free queue implementation.
//  Only a single thread can read from the pipe at any specific moment.
//  Only a single thread can write to the pipe at any specific moment.
//  T is the type of the object in the queue.
//  N is granularity of the pipe, i.e. how many items are needed to
//  perform next memory allocation.

template <typename T, int N>
class ypipe_t : public ypipe_base_t<T> {
public:
	//  Initialises the pipe.
	inline ypipe_t() {
		//  Insert terminator element into the queue.
		queue.push();

		//  Let all the pointers to point to the terminator.
		//  (unless pipe is dead, in which case c is set to NULL).
		r = w = f = &queue.back();
		c.set(&queue.back());
	}

	//  The destructor doesn't have to be virtual. It is made virtual
	//  just to keep ICC and code checking tools from complaining.
	inline virtual ~ypipe_t() {}

	//  Following function (write) deliberately copies uninitialised data

	//  Write an item to the pipe.  Don't flush it yet. If incomplete is
	//  set to true the item is assumed to be continued by items
	//  subsequently written to the pipe. Incomplete items are never
	//  flushed down the stream.
	//  TODO 使用完美转发
	inline void write(const T &value_, bool incomplete_) {
		//  Place the value to the queue, add new terminator element.
		queue.back() = value_;
		queue.push();

		//  Move the "flush up to here" poiter.
		if (!incomplete_) {
			f = &queue.back();
		}
	}

	//  value_ 返回 poped incomplete item
	//  Pop an incomplete item from the pipe. Returns true if such
	//  item exists, false otherwise.
	inline bool unwrite(T *value_) {
		if (f == &queue.back()) {
			return false;
		}
		queue.unpush();
		*value_ = queue.back();
		return true;
	}

	//  Flush all the completed items into the pipe. Returns false if
	//  the reader thread is sleeping. In that case, caller is obliged to
	//  wake the reader up before using the pipe again.
	inline bool flush() {
		//  If there are no un-flushed items, do nothing.
		if (w == f) {
			return true;
		}

		w = f;
		//  Try to set 'c' to 'f'.
		if (c.cas(w, f) != w) {
			//  Compare-and-swap was unseccessful because 'c' is NULL.
			//  This means that the reader is asleep. Therefore we don't
			//  care about thread-safeness and update c in non-atomic
			//  manner. We'll return false to let the caller know
			//  that reader is sleeping.
			//  'c' 为 NULL 表示 reader 没有 item 可读，所以会 asleep
			c.set(f);
			return false;
		}

		//  Reader is alive. Nothing special to do now. Just move
		//  the 'first un-flushed item' pointer to 'f'.
		return true;
	}

	//  Check whether item is available for reading.
	inline bool check_read() {
		//  Was the value prefetched already? If so, return.
		if (&queue.front() != r && r) {
			return true;
		}

		//  There's no prefetched value, so let us prefetch more values.
		//  Prefetching is to simply retrieve the
		//  pointer from c in atomic fashion. If there are no
		//  items to prefetch, set c to NULL (using compare-and-swap).
		r = c.cas(&queue.front(), NULL);

		//  If there are no elements prefetched, exit.
		//  During pipe's lifetime r should never be NULL, however,
		//  it can happen during pipe shutdown when items
		//  are being deallocated.
		//  正确用法是当read()返回false时 reader 进程进入asleep，等待writer()
		//  线程唤醒, 这样不会出现 r 为 NULL 的情况.
		//  若read()返回false，reader进程不sleep，继续read(), 且此时
		//  writer线程没有生产新元素时，指针 r 也会为 NULL.
		if (&queue.front() == r || !r) {
			return false;
		}

		//  There was at least one value prefetched.
		return true;
	}

	//  Reads an item from the pipe. Returns false if there is no value.
	//  available.
	inline bool read(T *value_) {
		//  Try to prefetch a value.
		if (!check_read()) {
			return false;
		}

		//  There was at least one value prefetched.
		//  Return it to the caller.
		*value_ = queue.front();
		queue.pop();
		return true;
	}

	//  Applies the function fn to the first elemenent in the pipe
	//  and returns the value returned by the fn.
	//  The pipe mustn't be empty or the function crashes.
	inline bool probe(bool (*fn)(const T &)) {
		bool rc = check_read();
		utils_assert(rc);

		return (*fn)(queue.front());
	}

protected:
	//  Allocation-efficient queue to store pipe items.
	//  Front of the queue points to the first prefetched item, back of
	//  the pipe points to last un-flushed item. Front is used only by
	//  reader thread, while back is used only by writer thread.
	yqueue_t<T, N> queue;

	//  调用 write() 之后，调用 flush() 之前的 item 都是 un-flushed item
	//  flushed item 才能被 read() 读到

	//  Points to the first un-flushed item. This variable is used
	//  exclusively by writer thread.
	T *w;

	//  该指针之前的 item 是 prefetched item, check_read() 更新
	//  Points to the first un-prefetched item. This variable is used
	//  exclusively by reader thread.
	T *r;

	//  "flush up to here" poiter
	//  incomplete item 是不能被 flush 的
	//  Points to the first item to be flushed in the future.
	T *f;

	//  flush() 和 check_read() 中会更新这个值, 使用 cas 机制 
	//  The single point of contention between writer and reader thread.
	//  Points past the last flushed item. If it is NULL,
	//  reader is asleep. This pointer should be always accessed using
	//  atomic operations.
	atomic_ptr_t<T> c;

	//  Disable copying of ypipe object.
	ypipe_t(const ypipe_t &);
	const ypipe_t &operator=(const ypipe_t &);
};
}

#endif
