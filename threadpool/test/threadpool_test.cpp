#include "../threadpool.hpp"

#include <iostream>

class Task {
public:
	Task() {  }

	static int c;

	void run() {
		long long i, sum = 0;
		for (i = 0; i < 10000000LL; ++i) {
			sum += i;
		}
		++c;
	}
};

int Task::c = 0;

int main()
{
	utils::Threadpool<Task> threadpool(20);
	threadpool.start();

	for (int i = 0; i < 4; ++i) {
		threadpool.addTask( Task() );
	}

	threadpool.shutdown();

	std::cout << Task::c << std::endl;

	return 0;
}
