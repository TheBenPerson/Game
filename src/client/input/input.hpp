#ifndef GAME_CLIENT_INPUT

#include "nodelist.hpp"
#include "point.hpp"

namespace Input {

	enum Actions {

		A_UP,
		A_DOWN,
		A_LEFT,
		A_RIGHT,
		A_EXIT,
		A_ACTION,
		A_FULLSCREEN,
		A_NUM_ACTIONS

	};

	extern bool actions[A_NUM_ACTIONS];
	extern Point mouse;
	extern NodeList listeners;

	extern void notify();

}

#define GAME_CLIENT_INPUT
#endif
