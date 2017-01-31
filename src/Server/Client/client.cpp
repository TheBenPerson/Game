#include <pthread.h>
#include <stdint.h>
#include "client.hpp"

Client::Client(int Socket) {

	this->Socket = Socket;

	pthread_attr_t Attrib;
	pthread_attr_init(&Attrib);

	pthread_create(&Thread, &Attrib, HandleClient, (void *) this);

	pthread_attr_destroy(&Attrib);

}

void * Client::HandleClient(void * Client) {



}

