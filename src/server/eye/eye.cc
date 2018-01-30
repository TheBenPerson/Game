#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "client.hh"
#include "console.hh"
#include "eye.hh"
#include "packet.hh"
#include "world.hh"

extern "C" {

	bool init() {

		Point pos = {-4, 0};
		Point vel = {0, 0};
		new Eye(pos, vel);

		pos = {4, 0};
		new Eye(pos, vel);

		/*pos = {-2.5f, -2.5f};
		vel = {.01f, .01f};
		new Eye(pos, vel);

		pos = {-2.5f, 2.5f};
		vel = {.01f, -.01f};
		new Eye(pos, vel);*/

		cputs(GREEN, "Loaded module: 'eye.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'eye.so'");

	}

}

Eye::Eye(Point pos, Point vel): Entity(pos, vel) {}

unsigned int Eye::tick(timespec *time) {

	time->tv_nsec /= 1000000;

	if (!lastNano) lastNano = time->tv_nsec;
	unsigned int dtime = time->tv_nsec - lastNano;

	if (dtime < 16) {

		lastNano = time->tv_nsec;
		return 16 - dtime;

	}

	// every 50 milis

	World::setTile(&pos, Tile::LAVA);
	pos += vel;

	lastNano = time->tv_nsec;

	if (!lastSec) lastSec = time->tv_sec;
	dtime = time->tv_sec - lastSec;

	if (dtime >= 2) {

		// every 2 sec

		float rot = (rand() / (float) RAND_MAX) * M_PI * 2;

		float x = cosf(rot) / 100;
		float y = sinf(rot) / 100;

		vel = { x, y };
		update();

		lastSec = time->tv_sec;

	}

	return 0;

}

char* Eye::getType() {

	return "eye";

}
