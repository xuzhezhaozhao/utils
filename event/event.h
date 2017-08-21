/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 14时32分48秒
 @File Name: event.h
 @Description:
 ******************************************************/

#include <functional>
#include <exception>
#include <list>

namespace utils{

enum {
	EV_NONE = 0,
	EV_READABLE = 1,
	EV_WRITEABLE = 2,
	END = 3,
};

// 两个参数分别为：fd, events
typedef std::function<void(int, int, void *)> event_handle_t;
typedef std::function<void(void *)> time_handle_t;

class event_error_t : std::exception {
public:
	explicit event_error_t(int err, const std::string &src);
};

class BaseEvent
{
public:

	BaseEvent() {  }
	virtual ~BaseEvent() {  }

	int init();

	int AddEvent(int fd, int events, void *args, event_handle_t event_handle);
	int AddTimerEvent(int timeout, void *args, time_handle_t time_handle);
	int DelEvent(int fd);
	int DelTimerEvent();
	void event_loop();

	std::string errmsg();

private:
	struct event_info {
		int fd;
		// 事件类型
		int events;
		// handle 的调用参数
		void *args;
		int valid;
		event_handle_t handle;
	};

	struct timer_info {
	};

	struct event_info *fpool(int fd) {
		return &event_info_vector_[fd];
	}

private:
	const unsigned int EVENT_INFO_VECTOR_INIT_SIZE = 64;
	int epfd_;
	// TODO 资源管理类 shared_ptr()
	struct event_info *event_info_vector_;
	int size_;

	std::list<timer_info> timer_list;
};

}	// namespace utils
