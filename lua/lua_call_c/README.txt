测试：
./lua test_hello.lua
./lua test_mylib.lua

# 这个会出错，需要修改 lua Makefile 添加 ext.o
./lua test_test_ext.lua

如果 lua 调用的 C 动态库中调用了其他静态库的函数, 解决办法是把相应的 .a 解压，
修改 lua Makefile, 生成 lua bin 文件时将所有 *.o 文件添加上，这样 lua require 
时可以调用到相关函数.

(不知道有没有更好的办法)
