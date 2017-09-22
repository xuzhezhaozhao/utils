/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月23日 星期三 23时10分42秒
 @File Name: pb_helper_test.cpp
 @Description:
 ******************************************************/

#include "../pb_helper.hpp"
#include "gtest/gtest.h"
#include "json/json.h"
#include "person.pb.h"

#include <iostream>

using namespace utils::pb_helper;

TEST(PB_HELPER, protobufToJsonString) {
	Person person;
	person.set_name("徐哲钊");
	person.set_id(3533);
	person.set_email("835997137@qq.com");

	Person::PhoneNumber* phone = person.add_phones();
	phone->set_number("13124738469");
	phone->set_type(Person::PhoneType::Person_PhoneType_MOBILE);

	phone = person.add_phones();
	phone->set_number("7776666");
	phone->set_type(Person::PhoneType::Person_PhoneType_HOME);

	phone = person.add_phones();
	phone->set_number("6668888");
	phone->set_type(Person::PhoneType::Person_PhoneType_WORK);

	person.mutable_ticket()->Add(19930326);
	person.mutable_ticket()->Add(19931021);
	person.mutable_ticket()->Add(19680801);
	person.mutable_ticket()->Add(19681006);

	person.mutable_addr()->Add("zhangshu");
	person.mutable_addr()->Add("wuhan");
	person.mutable_addr()->Add("beijing");
	person.mutable_addr()->Add("shenzheng");

	Json::CharReaderBuilder rbuilder;
	Json::CharReader* read = rbuilder.newCharReader();
	std::string pbstring = protobufToJsonString(&person);
	std::string err;
	Json::Value root;
	ASSERT_TRUE(read->parse(pbstring.data(), pbstring.data() + pbstring.size(),
							&root, &err));
	EXPECT_TRUE(root.isMember("name") && root["name"].isString() &&
				root["name"].asString() == person.name());
	EXPECT_TRUE(root.isMember("id") && root["id"].isInt() &&
				root["id"].asInt() == person.id());
	EXPECT_TRUE(root.isMember("email") && root["email"].isString() &&
				root["email"].asString() == person.email());

	ASSERT_TRUE(root.isMember("phones") && root["phones"].isArray() &&
				root["phones"].size() == 3);
	ASSERT_TRUE(root["phones"][0].isMember("number"));
	ASSERT_TRUE(root["phones"][0]["number"].isString());
	ASSERT_TRUE(root["phones"][0]["number"].asString() == "13124738469");
	ASSERT_TRUE(root["phones"][1].isMember("number"));
	ASSERT_TRUE(root["phones"][1]["number"].isString());
	ASSERT_TRUE(root["phones"][1]["number"].asString() == "7776666");
	ASSERT_TRUE(root["phones"][2].isMember("number"));
	ASSERT_TRUE(root["phones"][2]["number"].isString());
	ASSERT_TRUE(root["phones"][2]["number"].asString() == "6668888");

	ASSERT_TRUE(root.isMember("ticket"));
	ASSERT_TRUE(root["ticket"].isArray());
	ASSERT_TRUE(root["ticket"][0] == 19930326);
	ASSERT_TRUE(root["ticket"][1] == 19931021);
	ASSERT_TRUE(root["ticket"][2] == 19680801);
	ASSERT_TRUE(root["ticket"][3] == 19681006);

	ASSERT_TRUE(root.isMember("addr"));
	ASSERT_TRUE(root["addr"].isArray());
	ASSERT_TRUE(root["addr"][0] == "zhangshu");
	ASSERT_TRUE(root["addr"][1] == "wuhan");
	ASSERT_TRUE(root["addr"][2] == "beijing");
	ASSERT_TRUE(root["addr"][3] == "shenzheng");
}
