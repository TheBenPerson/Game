#ifndef GAME_MAIN

#include "nodelist.hpp"

typedef struct {

	void *handle;
	void (*cleanup)();

} Module;

extern NodeList modules;

#define GAME_MAIN
#endif
