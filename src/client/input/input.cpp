#include <stdio.h>
#include <string.h>

#include "client.hpp"
#include "console.hpp"
#include "input.hpp"

extern "C" {

	bool init() {

		cputs("Loaded module: 'input.so'");
		return true;

	}

	void cleanup() {

		cputs("Unloaded module: 'input.so'", RED);

	}

}

namespace Input {

	bool actions[A_NUM_ACTIONS];
	Point mouse;
	NodeList listeners;

	void notify() {

		for (unsigned int i = 0; i < listeners.len; i++)
			((void (*)()) listeners.get(i))();

	}

}
