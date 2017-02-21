#ifndef HG_Client_H

#include <pthread.h>
#include <X11/Xlib.h>
#include "Util/Config/config.hpp"

namespace Client {

	extern Config config;
	extern bool running;
	extern pthread_t thread;

	void cleanup();
	bool init();
	bool start();
	void * threadMain(void*);
	void tick();

}

#define HG_Client_H
#endif
