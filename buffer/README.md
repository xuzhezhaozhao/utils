# test with valgrind
build$ cmake -DCMAKE_BUILD_TYPE=Debug ..
build$ valgrind --leak-check=yes test/buffer_test


# cmake ctest
build$ make test ARGS=-V
