#ifndef HG_Server_H

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>

namespace Server {

	extern bool local;
	extern bool running;
	extern pthread_t thread;

	void cleanup();
	bool init(uint16_t port);
	void * tickLoop(void*);
	bool start(bool local, uint16_t port);
	void stop();

}

#define HG_Server_H
#endif
