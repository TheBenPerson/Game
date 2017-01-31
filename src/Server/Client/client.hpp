#ifndef HG_Server_Client_H

#include <pthread.h>

class Client {

	public:

		Client(int Socket);
		static void * HandleClient(void * Client);

	private:

		int Socket;
		pthread_t Thread;

};

#define HG_Server_Client_H
#endif
