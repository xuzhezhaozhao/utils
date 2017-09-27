/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Thu 05 Oct 2017 04:29:13 PM CST
 @File Name: rtdealer.c
 @Description: p97
 与 router_req 例子类似，使用 ZMQ_DEALER 替换 ZMQ_REQ，并保持与
 ZMQ_REQ 的兼容
 ******************************************************/

//  ROUTER-to-DEALER example

#include <pthread.h>
#include "../zhelpers.h"
#define NBR_WORKERS 10

static void *worker_task(void *args) {
	(void)args;
	void *context = zmq_ctx_new();
	void *worker = zmq_socket(context, ZMQ_DEALER);

	s_set_id(worker);  //  Set a printable identity

	zmq_connect(worker, "tcp://localhost:5671");

	int total = 0;
	while (1) {
		// ZMQ_DEALER 与 ZMQ_REQ 的差别在于 ZMQ_DEALER 需要手工接收/发送帧分隔符
		 
		//  Tell the broker we're ready for work
		s_sendmore(worker, "");
		s_send(worker, "Hi Boss");

		//  Get workload from broker, until finished
		free(s_recv(worker));  //  Envelope delimiter
		char *workload = s_recv(worker);
		//  .skip
		int finished = (strcmp(workload, "Fired!") == 0);
		free(workload);
		if (finished) {
			printf("Completed: %d tasks\n", total);
			break;
		}
		total++;

		//  Do some random work
		s_sleep(randof(500) + 1);
	}
	zmq_close(worker);
	zmq_ctx_destroy(context);
	return NULL;
}

//  .split main task
//  While this example runs in a single process, that is just to make
//  it easier to start and stop the example. Each thread has its own
//  context and conceptually acts as a separate process.

int main(void) {
	void *context = zmq_ctx_new();
	void *broker = zmq_socket(context, ZMQ_ROUTER);

	zmq_bind(broker, "tcp://*:5671");
	srandom((unsigned)time(NULL));

	int worker_nbr;
	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr++) {
		pthread_t worker;
		pthread_create(&worker, NULL, worker_task,
					   (void *)(intptr_t)worker_nbr);
	}
	//  Run for five seconds and then tell workers to end
	int64_t end_time = s_clock() + 5000;
	int workers_fired = 0;
	while (1) {
		//  Next message gives us least recently used worker
		char *identity = s_recv(broker);
		s_sendmore(broker, identity);
		free(identity);
		free(s_recv(broker));  //  Envelope delimiter
		free(s_recv(broker));  //  Response from worker
		s_sendmore(broker, "");

		//  Encourage workers until it's time to fire them
		if (s_clock() < end_time)
			s_send(broker, "Work harder");
		else {
			s_send(broker, "Fired!");
			if (++workers_fired == NBR_WORKERS) break;
		}
	}
	zmq_close(broker);
	zmq_ctx_destroy(context);
	return 0;
}
//  .until
