/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月31日 星期四 22时14分21秒
 @File Name: jsconcpp_helper_test.cpp
 @Description:
 ******************************************************/

#include "../jsoncpp_helper.h"
#include "gtest/gtest.h"
#include "json/json.h"

TEST(JSONCPP_HELPER, check_basic) {
	using namespace utils::jsoncpp_helper;
	Json::Value value;
	ASSERT_TRUE(!checkJsonArgs(value, "id", int32_tag));
	value["id"] = 158;
	ASSERT_TRUE(checkJsonArgs(value, "id", int32_tag));
	ASSERT_TRUE(checkJsonArgs(value, "id", uint32_tag));
	ASSERT_TRUE(checkJsonArgs(value, "id", int64_tag));
	ASSERT_TRUE(checkJsonArgs(value, "id", uint64_tag));
	ASSERT_TRUE(checkJsonArgs(value, "id", numeric_tag));

	ASSERT_TRUE(!checkJsonArgs(value, "name", string_tag));
	value["name"] = "zhezhaoxu";
	ASSERT_TRUE(checkJsonArgs(value, "name", string_tag));
	ASSERT_TRUE(!checkJsonArgs(value, "name", numeric_tag));

	ASSERT_TRUE(!checkJsonArgs(value, "isgood", bool_tag));
	value["isgood"] = true;
	ASSERT_TRUE(checkJsonArgs(value, "isgood", bool_tag));
	ASSERT_TRUE(!checkJsonArgs(value, "isgood", int32_tag));

	ASSERT_TRUE(!checkJsonArgs(value, "score", double_tag));
	value["score"] = 3.0;
	ASSERT_TRUE(checkJsonArgs(value, "score", double_tag));
	ASSERT_TRUE(checkJsonArgs(value, "score", int32_tag));

	value["score"] = 3.2;
	ASSERT_TRUE(checkJsonArgs(value, "score", double_tag));
	ASSERT_TRUE(!checkJsonArgs(value, "score", int32_tag));

	ASSERT_TRUE(!checkJsonArgs(value, "cars", array_tag));
	value["cars"] = "bwm";
	ASSERT_TRUE(!checkJsonArgs(value, "cars", array_tag));
	value["cars"] = Json::arrayValue;
	ASSERT_TRUE(checkJsonArgs(value, "cars", array_tag));
	value["cars"][0] = "bwm";
	ASSERT_TRUE(checkJsonArgs(value, "cars", array_tag));
	value["cars"][1] = "benz";
	ASSERT_TRUE(checkJsonArgs(value, "cars", array_tag));
	ASSERT_TRUE(!checkJsonArgs(value, "cars", object_tag));

	value["feed"] = Json::objectValue;
	ASSERT_TRUE(checkJsonArgs(value, "feed", object_tag));
	value["feed"]["title"] = "let's go";
	ASSERT_TRUE(checkJsonArgs(value, "feed", object_tag));
	ASSERT_TRUE(!checkJsonArgs(value, "feed", array_tag));
}

TEST(JSONCPP_HELPER, check_advanced) {
	using namespace utils::jsoncpp_helper;
	Json::Value value;
	value["id"] = 158;
	value["qq"] = 123456;
	value["wechat"] = "abc_abc";
	value["null"] = Json::nullValue;
	value["score"] = 95.5;
	value["list"][0] = 1;
	value["list"][1] = 2;
	value["obj"]["home"] = "zhangshu";

	ASSERT_TRUE(checkJsonArgs(
		value, "id", int32_tag, "qq", uint32_tag, "wechat", string_tag, "null",
		null_tag, "score", double_tag, "list", array_tag, "obj", object_tag));

	ASSERT_TRUE(!checkJsonArgs(value, "addr", string_tag));
}

TEST(JSONCPP_HELPER, setvalue_basic) {
	using namespace utils::jsoncpp_helper;
	Json::Value value;
	setJsonValue(value, "name", "zhezhaoxu");
	ASSERT_TRUE(checkJsonArgs(value, "name", string_tag));
	ASSERT_TRUE(value["name"] == "zhezhaoxu");

	setJsonValue(value, "id", 158);
	ASSERT_TRUE(checkJsonArgs(value, "id", int32_tag));
	ASSERT_TRUE(value["id"] == 158);

	setJsonValue(value, "good", true);
	ASSERT_TRUE(checkJsonArgs(value, "good", bool_tag));
	ASSERT_TRUE(value["good"] == true);

	setJsonValue(value, "score", 95.5);
	ASSERT_TRUE(checkJsonArgs(value, "score", double_tag));
	ASSERT_TRUE(value["score"] == 95.5);

	Json::Value subvalue;
	subvalue["name"] = "zs";
	subvalue["id"] = 10086;
	subvalue["addr"] = "zhangshu";
	setJsonValue(value, "school", subvalue);
	ASSERT_TRUE(checkJsonArgs(value, "school", object_tag));
	ASSERT_TRUE(value["school"] == subvalue);
}

TEST(JSONCPP_HELPER, setvalue_advanced) {
	using namespace utils::jsoncpp_helper;
	Json::Value value;
	setJsonValue(value, "name", "zhezhaoxu", "id", 158, "good", true, "score",
				 95.5);
	ASSERT_TRUE(checkJsonArgs(value, "name", string_tag, "id", int32_tag,
							  "good", bool_tag, "score", double_tag));
	ASSERT_TRUE(value["name"] == "zhezhaoxu");
	ASSERT_TRUE(value["id"] == 158);
	ASSERT_TRUE(value["good"] == true);
	ASSERT_TRUE(value["score"] == 95.5);
}

TEST(JSONCPP_HELPER, setarrayValue) {
	using namespace utils::jsoncpp_helper;
	Json::Value value;
	setJsonArrayValue(value["array"], 158, 19930326, "zhezhao", true, 95.5);
	ASSERT_TRUE(checkJsonArgs(value, "array", array_tag));
	ASSERT_TRUE(checkJsonArgs(value["array"][0] == 158));
	ASSERT_TRUE(checkJsonArgs(value["array"][1] == 19930326));
	ASSERT_TRUE(checkJsonArgs(value["array"][2] == "zhezhao"));
	ASSERT_TRUE(checkJsonArgs(value["array"][3] == true));
	ASSERT_TRUE(checkJsonArgs(value["array"][4] == 95.5));

	std::vector<int> tickets = {1, 2, 3};
	setJsonArrayValue(value["tickets"], tickets, 3);
	ASSERT_TRUE(checkJsonArgs(value, "tickets", array_tag));
	EXPECT_TRUE(value["tickets"][0] == 1);
	EXPECT_TRUE(value["tickets"][1] == 2);
	EXPECT_TRUE(value["tickets"][2] == 3);

	setJsonArrayValue(value["info"], "zhezhaoxu", 24);
	ASSERT_TRUE(checkJsonArgs(value, "info", array_tag));
	EXPECT_TRUE(value["info"][0] == "zhezhaoxu");
	EXPECT_TRUE(value["info"][1] == 24);

	int tags[] = {0, 1, 2};
	setJsonArrayValue(value["tags"], tags, sizeof(tags) / sizeof(int));
	ASSERT_TRUE(checkJsonArgs(value, "tags", array_tag));
	EXPECT_TRUE(value["tags"][0] == 0);
	EXPECT_TRUE(value["tags"][1] == 1);
	EXPECT_TRUE(value["tags"][2] == 2);

	char names[128][128] = { "xzz", "hjc", "bob", "ama" };
	setJsonArrayValue(value["names"], names, 4);
	ASSERT_TRUE(checkJsonArgs(value, "names", array_tag));
	EXPECT_TRUE(value["names"][0] == "xzz");
	EXPECT_TRUE(value["names"][1] == "hjc");
	EXPECT_TRUE(value["names"][2] == "bob");
	EXPECT_TRUE(value["names"][3] == "ama");
}

