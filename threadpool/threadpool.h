/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月18日
 @File Name: threadpool.h
 @Description: 线程池
 ******************************************************/

#ifndef UTILS_THREADPOOL_THREADPOOL_H_
#define UTILS_THREADPOOL_THREADPOOL_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <iostream>

namespace utils
{

class Runable 
{
	virtual void run() = 0;
};

template <typename T = Runable, typename U = std::queue<T> >
class Threadpool 
{
public:
	typedef T task_type;
	typedef U queue_type;

	explicit Threadpool(int size = 1) : size_(size) { }

	Threadpool(const Threadpool &) = delete;
	Threadpool & operator=(const Threadpool &) = delete;

	~Threadpool() {  }

	void start() {
		shutdown_ = false;
		threads_.reserve(size_);
		for (int i = 0; i < size_; ++i) {
			threads_.emplace_back(&Threadpool::threadfunc, this);
		}
	}

	void addTask(const task_type &task) {
		std::unique_lock<std::mutex> lck(mtx_);
		queue_.push(task);
		lck.unlock();
		cv_.notify_one();
	}

	void threadfunc() {
		// 从队列头中取一个任务执行
		while (true) {
			std::unique_lock<std::mutex> lck(mtx_);
			while (queue_.size() == 0 && !shutdown_) {
				cv_.wait(lck);
			}

			if (queue_.size() == 0) {
				// TODO 网上这里写的是 if (shutdown_)
				// 与网上有点不一样，会将任务池中的任务全部完成后才shutdown
				// notified by shutdown() or shutdown_ is set
				// 队列中没有任务，可以shutdown
				break;
			}

			auto task = queue_.front();
			queue_.pop();
			lck.unlock();

			task.run();
		}
	}

	void shutdown() {
		{
			// TODO 网上某些版本这里没有加锁
			std::lock_guard<std::mutex> lck(mtx_);
			shutdown_ = true;
			cv_.notify_all();
		}
		for (auto & th : threads_) {
			th.join();
		}
	}

private:
	std::vector<std::thread> threads_;
	queue_type queue_;
	int size_;

	std::mutex mtx_;
	std::condition_variable cv_;
	bool shutdown_;
};

}

#endif /* ifndef UTILS_THREADPOOL_THREADPOOL_H_ */
