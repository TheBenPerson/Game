#ifndef HG_Client_H

#include <pthread.h>
#include <X11/Xlib.h>

namespace Client {

	extern bool running;
	extern pthread_t thread;

	void cleanup();
	bool init();
	bool start();
	void tick();
	void* tickLoop(void*);

}

#define HG_Client_H
#endif
