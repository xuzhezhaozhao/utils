/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Mon 25 Sep 2017 01:37:59 PM CST
 @File Name: tcp_address_test.cpp
 @Description:
 ******************************************************/


#include "../tcp_address.hpp"

#include <gtest/gtest.h>
#include <string>

TEST(TCP_ADDRESS, basic) {

	utils::tcp_address_t tcp_address;
	tcp_address.resolve("192.168.0.1:7070", false, false);
	std::string name;
	std::cout << tcp_address.to_string(name) << std::endl;
	std::cout << name << std::endl;

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
