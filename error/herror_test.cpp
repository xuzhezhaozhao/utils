/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月22日 星期二 22时21分59秒
 @File Name: herror_test.cpp
 @Description:
 ******************************************************/

#include "herror.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

void error(const char *location, const char *msg)
{
  printf("Error at %s: %s\n", location, msg);
}

void test_print_stack() {
		//utils::herror::print_stack_fd();
	std::cout <<  utils::herror::get_stackinfo() << std::endl;
}

int main()
{
	if (open("non_exit", O_RDONLY)) {
		std::cout << utils::herror::err_atstr("open failed") << std::endl;
	}
	test_print_stack();
	
	return 0;
}
