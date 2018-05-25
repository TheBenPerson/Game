#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "client.hh"
#include "console.hh"
#include "entity.hh"
#include "point.hh"
#include "human.hh"
#include "tile.hh"
#include "world.hh"

extern "C" {

	bool init() {

		cputs(GREEN, "Loaded module: 'human.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'human.so'");

	}

}

Human::Human(Client *client): Entity(client) {

	type = "Human";
	dim = {1, 1};

	interval = 1000;
	send();

}

bool Human::tick(timespec *time) {

	return false;

}
