/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 14时32分48秒
 @File Name: zevent.h
 @Description:
 ******************************************************/

#ifndef UTILS_EVENT_ZEVENT_H_
#define UTILS_EVENT_ZEVENT_H_

#include <exception>
#include <functional>
#include <list>
#include <string>

namespace utils {
namespace zevent {

#define Z_OK 0
#define Z_ERR -1

#define Z_NONE 0
#define Z_READABLE 1
#define Z_WRITEABLE 2

#define Z_FILE_EVENTS 1
#define Z_TIME_EVENTS 2
#define Z_ALL_EVENTS (Z_FILE_EVENTS|Z_TIME_EVENTS)
#define Z_DONT_WAIT 4

// 三个参数分别为：fd, clientData, mask
typedef std::function<void(int, void*, int)> zFileProc;

class BaseEvent {
   public:
	BaseEvent() {}
	virtual ~BaseEvent();

	int init(int setsize);

	int CreateFileEvent(int fd, int mask, zFileProc proc, void* clientData);
	int DeleteFileEvent(int fd, int mask);
	int GetFileEvents(int fd);
	int ProcessEvents(int flags);

	int getSetSize() const { return setsize_; }
	int resizeSetsize(int setsize);

	const std::string& errmsg() const { return errmsg_; }

   private:
	struct zFileEvent {
		int mask;  // Z_(READABLE|WRITEABLE)
		zFileProc fileProc;
		void* clientData;
	};
	// 就绪事件
	struct zFiredEvent {
		int fd;
		int mask;
	};

	int zApiCreate();
	int zApiResize(int setsize);
	void zApiFree();
	int zApiAddEvent(int fd, int mask);
	int zApiDelEvent(int fd, int delmask);
	int zApiPoll(struct timeval* tvp);
	const char* zApiName() const;


   private:
	int maxfd_;			 // 当前注册最大 fd 值
	int setsize_;		 // 当前注册的 fd 数目
	zFileEvent* events_;  // 注册事件
	zFiredEvent* fired_;  // 就绪事件
	void* apidata_;		 // 与 polling API 相关的数据结构

	std::string errmsg_;
};

}  // namespace zevent
}  // namespace utils

#endif /* ifndef UTILS_EVENT_EVENT_H_ */
