#ifndef GAME_CLIENT_INPUT

#include "nodelist.hh"
#include "point.hh"

namespace Input {

	typedef enum {

		A_UP,
		A_DOWN,
		A_LEFT,
		A_RIGHT,
		A_EXIT,
		A_ACTION,
		A_FULLSCREEN,
		A_NUM_ACTIONS

	} Action;

	extern bool actions[A_NUM_ACTIONS];
	extern Point cursor;
	extern bool wasCursor;
	extern NodeList listeners;

	extern void notify();

}

#define GAME_CLIENT_INPUT
#endif
