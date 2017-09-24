//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "inproc://test");
    assert (rc == 0);

    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "inproc://test");


	printf("binded\n");

	char buffer [10];
	zmq_send (requester, "Hello", 5, 0);
	zmq_recv (responder, buffer, 10, 0);
	printf ("Received %s\n", buffer);
	zmq_send (responder, "World", 5, 0);
	zmq_recv (requester, buffer, 10, 0);
	printf("Received %s\n", buffer);

    return 0;
}
