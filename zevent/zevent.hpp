/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 14时32分48秒
 @File Name: zevent.hpp
 @Description:
 ******************************************************/

#ifndef UTILS_EVENT_ZEVENT_H_
#define UTILS_EVENT_ZEVENT_H_

#include <functional>
#include <string>

namespace utils {
namespace zevent {

static const int Z_OK = 0;
static const int Z_ERR = -1;

static const unsigned int Z_NONE = 0;
static const unsigned int Z_READABLE = 1;
static const unsigned int Z_WRITEABLE = 2;

// ProcessEvents(int flags)
// @flags
static const unsigned int Z_FILE_EVENTS = 1;
static const unsigned int Z_TIME_EVENTS = 2;
static const unsigned int Z_ALL_EVENTS = (Z_FILE_EVENTS | Z_TIME_EVENTS);
static const unsigned int Z_DONT_WAIT = 4;

// 文件事件回调函数类型
typedef void (*zFileProc)(int fd, void* clientData, int mask);

class BaseEvent {
   public:
	BaseEvent() {}
	virtual ~BaseEvent();

	int init(int setsize);
	int CreateFileEvent(int fd, int mask, zFileProc proc, void* clientData);
	int DeleteFileEvent(int fd, int mask);
	int GetFileEvents(int fd);
	int ProcessEvents(int flags);

	int GetSetsize() const { return setsize_; }
	int ResizeSetsize(int setsize);
	const char* GetAPIName() const { return zApiName(); }

	const std::string& errmsg() const { return errmsg_; }

   private:
	struct zFileEvent {
		int mask;  // 注册事件， Z_(READABLE|WRITEABLE)
		// 当读写事件的回调函数是同一个时，该函数在读写事件同时发生时只会调用一次
		zFileProc rfileProc;  // 读事件回调函数
		zFileProc wfileProc;  // 写事件回调函数
		void* clientData;	 // 回调函数的附加参数
	};
	// 就绪事件结构体
	struct zFiredEvent {
		int fd;
		int mask;  // 就绪事件
	};

	// 多路复用IO相关的API实现函数
	int zApiCreate();
	int zApiResize(int setsize);
	void zApiFree();
	int zApiAddEvent(int fd, int mask);
	int zApiDelEvent(int fd, int delmask);
	int zApiPoll(struct timeval* tvp);
	const char* zApiName() const;

   private:
	int maxfd_ = -1;   // 当前注册事件最大 fd 值
	int setsize_ = 0;  // 当前注册事件的 fd 数目

	zFileEvent* events_ = nullptr;  // 注册事件列表
	zFiredEvent* fired_ = nullptr;  // 就绪事件列表
	void* apidata_ = nullptr;		// 与 polling API 相关的数据结构

	std::string errmsg_;  // 错误信息
};

}  // namespace zevent
}  // namespace utils

#endif /* ifndef UTILS_EVENT_EVENT_H_ */
