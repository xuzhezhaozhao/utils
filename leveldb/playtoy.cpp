/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 31 Oct 2017 11:25:27 PM CST
 @File Name: playtoy.cpp
 @Description:
 ******************************************************/

#include <cassert>
#include "leveldb/db.h"


int main(int argc, char *argv[]) {
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);
	assert(status.ok());
	return 0;
}
