/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月23日 星期三 23时10分42秒
 @File Name: pb_helper_test.cpp
 @Description:
 ******************************************************/


#include "../pb_helper.h"
#include "proto/person.pb.h"
#include "gtest/gtest.h"

#include <iostream>

using namespace utils::pb_helper;

TEST(PB_HELPER, debug)
{
	Person person;
	person.set_email("835997137@qq.com 徐哲钊");

	std::cout << pb2Json(&person) << std::endl;
}
