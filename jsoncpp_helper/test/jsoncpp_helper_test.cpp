/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月31日 星期四 22时14分21秒
 @File Name: jsconcpp_helper_test.cpp
 @Description:
 ******************************************************/

#include "../jsoncpp_helper.h"
#include "gtest/gtest.h"
#include "json/json.h"

TEST(JSONCPP_HELPER, basic) {
	Json::Value value;
	ASSERT_TRUE(!checkJsoncppArgs(value, "id", jsoncpp_helper::int32_tag));
	value["id"] = 158;
	ASSERT_TRUE(checkJsoncppArgs(value, "id", jsoncpp_helper::int32_tag));
	ASSERT_TRUE(checkJsoncppArgs(value, "id", jsoncpp_helper::uint32_tag));
	ASSERT_TRUE(checkJsoncppArgs(value, "id", jsoncpp_helper::int64_tag));
	ASSERT_TRUE(checkJsoncppArgs(value, "id", jsoncpp_helper::uint64_tag));
	ASSERT_TRUE(checkJsoncppArgs(value, "id", jsoncpp_helper::numeric_tag));

	ASSERT_TRUE(!checkJsoncppArgs(value, "name", jsoncpp_helper::string_tag));
	value["name"] = "zhezhaoxu";
	ASSERT_TRUE(checkJsoncppArgs(value, "name", jsoncpp_helper::string_tag));
	ASSERT_TRUE(!checkJsoncppArgs(value, "name", jsoncpp_helper::numeric_tag));

	ASSERT_TRUE(!checkJsoncppArgs(value, "isgood", jsoncpp_helper::bool_tag));
	value["isgood"] = true;
	ASSERT_TRUE(checkJsoncppArgs(value, "isgood", jsoncpp_helper::bool_tag));
	ASSERT_TRUE(!checkJsoncppArgs(value, "isgood", jsoncpp_helper::int32_tag));

	ASSERT_TRUE(!checkJsoncppArgs(value, "score", jsoncpp_helper::double_tag));
	value["score"] = 3.0;
	ASSERT_TRUE(checkJsoncppArgs(value, "score", jsoncpp_helper::double_tag));
	ASSERT_TRUE(checkJsoncppArgs(value, "score", jsoncpp_helper::int32_tag));

	value["score"] = 3.2;
	ASSERT_TRUE(checkJsoncppArgs(value, "score", jsoncpp_helper::double_tag));
	ASSERT_TRUE(!checkJsoncppArgs(value, "score", jsoncpp_helper::int32_tag));

	ASSERT_TRUE(!checkJsoncppArgs(value, "cars", jsoncpp_helper::array_tag));
	value["cars"] = "bwm";
	ASSERT_TRUE(!checkJsoncppArgs(value, "cars", jsoncpp_helper::array_tag));
	value["cars"] = Json::arrayValue;
	ASSERT_TRUE(checkJsoncppArgs(value, "cars", jsoncpp_helper::array_tag));
	value["cars"][0] = "bwm";
	ASSERT_TRUE(checkJsoncppArgs(value, "cars", jsoncpp_helper::array_tag));
	value["cars"][1] = "benz";
	ASSERT_TRUE(checkJsoncppArgs(value, "cars", jsoncpp_helper::array_tag));
	ASSERT_TRUE(!checkJsoncppArgs(value, "cars", jsoncpp_helper::object_tag));

	value["feed"] = Json::objectValue;
	ASSERT_TRUE(checkJsoncppArgs(value, "feed", jsoncpp_helper::object_tag));
	value["feed"]["title"] = "let's go";
	ASSERT_TRUE(checkJsoncppArgs(value, "feed", jsoncpp_helper::object_tag));
	ASSERT_TRUE(!checkJsoncppArgs(value, "feed", jsoncpp_helper::array_tag));
}

TEST(JSONCPP_HELPER, advanced) {
	using namespace jsoncpp_helper;
	Json::Value value;
	value["id"] = 158;
	value["qq"] = 123456;
	value["wechat"] = "abc_abc";
	value["null"] = Json::nullValue;
	value["score"] = 95.5;
	value["list"][0] = 1;
	value["list"][1] = 2;
	value["obj"]["home"] = "zhangshu";

	assert(checkJsoncppArgs(value, "id", int32_tag, "qq", uint32_tag, "wechat",
							string_tag, "null", null_tag, "score", double_tag,
							"list", array_tag, "obj", object_tag));

	assert(!checkJsoncppArgs(value, "addr", string_tag));
}
