/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月21日 星期一 18时50分52秒
 @File Name: event_test.cpp
 @Description:
 ******************************************************/

#include "../zevent.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

int main() {
	utils::zevent::BaseEvent base_event;

	if (base_event.init(1024) < 0) {
		std::cout << base_event.errmsg() << std::endl;
		exit(-1);
	}

	return 0;
}
