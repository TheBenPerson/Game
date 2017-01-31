#ifndef HG_Client_Networking_H

#include <pthread.h>
#include <inttypes.h>

namespace Networking {

	extern bool Running;
	extern int Socket;
	extern pthread_t Thread;

	void Cleanup();
	void * PacketHandler(void *);
	bool Init(uint16_t Port);
	void Start(char* address);
	void Stop();

}

#define HG_Client_Networking_H
#endif
