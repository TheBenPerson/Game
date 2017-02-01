#ifndef HG_Client_Networking_H

#include <pthread.h>
#include <inttypes.h>

namespace Networking {

	extern bool running;
	extern int socket;
	extern pthread_t thread;

	void cleanup();
	void* packetHandler(void*);
	bool init(uint16_t port);
	void start(char* address);
	void stop();

}

#define HG_Client_Networking_H
#endif
