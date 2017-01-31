#ifndef HG_Server_Networking_H

#include <pthread.h>

namespace Networking {

	extern bool Running;
	extern int Socket;
	extern pthread_t Thread;

	void Cleanup();
	void * PacketHandler(void *);
	bool Init(uint16_t Port);
	void Start();
	void Stop();

}

#define HG_Server_Networking_H
#endif
