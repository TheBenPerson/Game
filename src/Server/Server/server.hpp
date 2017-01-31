#ifndef HG_Server_H

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>

namespace Server {

	extern bool Local;
	extern bool Running;
	extern pthread_t Thread;

	void Cleanup();
	bool Init(uint16_t Port);
	void * TickLoop(void *);
	bool Start(bool Local, uint16_t Port);
	void Stop();

}

#define HG_Server_H
#endif
