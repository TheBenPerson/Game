#include <stdio.h>
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "input.hh"

extern "C" {

	bool init() {

		cputs(GREEN, "Loaded module: 'input.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'input.so'");

	}

}

namespace Input {

	bool actions[A_NUM_ACTIONS];
	Point cursor;
	bool wasCursor;
	NodeList listeners;

	void notify() {

		for (unsigned int i = 0; i < listeners.len; i++)
			((void (*)()) listeners.get(i))();

	}

}
