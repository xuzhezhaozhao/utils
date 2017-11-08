/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Tue 31 Oct 2017 11:25:27 PM CST
 @File Name: playtoy.cpp
 @Description:
 ******************************************************/

#include <cassert>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

#include <iostream>

int main(int argc, char* argv[]) {
	// Open leveldb
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);
	if (!status.ok()) {
		std::cout << status.ToString() << std::endl;
		exit(-1);
	}

	// Reads and Writes
	std::string key1 = "name";
	status = db->Put(leveldb::WriteOptions(), key1, "zhezhaoxu");
	if (!status.ok()) {
		std::cout << status.ToString() << std::endl;
		exit(-1);
	}
	std::string value;
	status = db->Get(leveldb::ReadOptions(), key1, &value);
	if (!status.ok()) {
		std::cout << status.ToString() << std::endl;
		exit(-1);
	}

	std::cout << key1 << ": " << value << std::endl;

	// Atomic Updates
	std::string key2 = "author";
	leveldb::WriteBatch batch;
	batch.Delete(key1);
	batch.Put(key2, value);
	status = db->Write(leveldb::WriteOptions(), &batch);
	if (!status.ok()) {
		std::cout << status.ToString() << std::endl;
		exit(-1);
	}

	status = db->Get(leveldb::ReadOptions(), key2, &value);
	if (!status.ok()) {
		std::cout << status.ToString() << std::endl;
		exit(-1);
	}

	std::cout << key2 << ": " << value << std::endl;

	// # Synchronous Writes
	// By default, each write to leveldb is asynchronous: it returns after
	// pushing the write from the process into the operating system. The
	// transfer from operating system memory to the underlying persistent
	// storage happens asynchronously. The sync flag can be turned on for a
	// particular write to make the write operation not return until the data
	// being written has been pushed all the way to persistent storage. (On
	// Posix systems, this is implemented by calling either fsync(...) or
	// fdatasync(...) or msync(..., MS_SYNC) before the write operation
	// returns.)

	leveldb::WriteOptions write_options;
	write_options.sync = true;
	status = db->Put(write_options, "sex", "male");
	if (!status.ok()) {
		std::cout << status.ToString() << std::endl;
		exit(-1);
	}

	// Iteration
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		std::cout << it->key().ToString() << ": " << it->value().ToString()
				  << std::endl;
	}
	assert(it->status().ok());  // Check for any errors found during the scan
	delete it;

	// for (it->Seek(start);
	// it->Valid() && it->key().ToString() < limit;
	// it->Next()) {
	// ...
	// }

	// for (it->SeekToLast(); it->Valid(); it->Prev()) {
	// ...
	// }

	// # Snapshots

	// leveldb::ReadOptions options;
	// options.snapshot = db->GetSnapshot();
	//... apply some updates to db... leveldb::Iterator* iter =
	// db->NewIterator(options);
	//... read using iter to view the state when the snapshot was
	// created... delete iter;
	// db->ReleaseSnapshot(options.snapshot);
	

	// # Slice
	leveldb::Slice s1 = "hello";
	std::string str("world");
	leveldb::Slice s2 = str;
	str = s1.ToString();
	assert(str == std::string("hello"));


	// # comparator
	// TODO
	// 
	// # Cache
	// TODO
	// 
	// # Filters
	// TODO

	delete db;
	return 0;
}
