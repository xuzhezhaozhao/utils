/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 18时50分52秒
 @File Name: event_test.cpp
 @Description:
 ******************************************************/

#include "event.h"

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void handle(int fd, int /* events */, void * /* args */) {
	std::cout << "handle events, fd = " << fd << std::endl;
}

int main()
{
	utils::BaseEvent base_event;

	if (base_event.init() < 0) {
		std::cout << "init() error" << std::endl;
		exit(-1);
	}

	int fd = open("../event.h", O_RDONLY);
	if (fd < 0) {
		std::cout << "open error" << std::endl;
		exit(-1);
	}

	base_event.AddEvent(fd, /* utils::EV_READABLE | */ utils::EV_WRITEABLE, NULL, handle);
	base_event.event_loop();

	return 0;
}
