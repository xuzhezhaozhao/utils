#! /usr/bin/env bash

echo "building deps/lua-5.3.4 ..."
pre_dir=`pwd`
cd deps/lua-5.3.4 && mkdir -p build && cd build && cmake .. && make
cd ../ && make linux
cd $pre_dir


echo "building deps/lua ..."
pre_dir=`pwd`
cd deps/lua && make CC=gcc
cd $pre_dir

echo "building deps/gflags ..."
pre_dir=`pwd`
cd deps/gflags && mkdir -p build && cd build && cmake .. && make
cd $pre_dir

echo "building deps/googletest ..."
pre_dir=`pwd`
cd deps/googletest && mkdir -p build && cd build && cmake .. && make
cd $pre_dir

echo "building deps/libzmq ..."
pre_dir=`pwd`
cd deps/libzmq && mkdir -p build && cd build && cmake .. && make
cd $pre_dir

echo "building deps/protobuf ..."
pre_dir=`pwd`
cd deps/protobuf && ./autogen.sh && ./configure && make
cd $pre_dir

echo "building deps/glog ..."
pre_dir=`pwd`
cd deps/glog && ./autogen.sh && ./configure && make
cd $pre_dir

echo "building deps/jsoncpp ..."
pre_dir=`pwd`
cd deps/jsoncpp && python amalgamate.py
cd $pre_dir


echo "building deps/leveldb ..."
pre_dir=`pwd`
cd deps/leveldb && make
cd $pre_dir
