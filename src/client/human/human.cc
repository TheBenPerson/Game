#include <GL/gl.h>
#include <math.h>
#include <stdint.h>

#include "console.hh"
#include "gfx.hh"
#include "net.hh"
#include "human.hh"
#include "world.hh"

GFX::texture tex;

static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("villager_f.png");
		Net::listeners.add((intptr_t) &tickNet);

		cputs(GREEN, "Loaded module: 'human.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((intptr_t) &tickNet);
		GFX::freeTexture(&tex);

		cputs(YELLOW, "Unloaded module: 'human.so'");

	}

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GENT: {

			Entity::UPacket *upacket = (Entity::UPacket *) packet->data;

			if (!Entity::verify(upacket, "Human")) return false;
			new Human((void*) upacket);

		} break;

		default: return false;

	}

	return true;

}

Human::Human(void *info): Entity(info) {}

void Human::draw() {

	Entity::draw();

	Point pvel = vel;
	pvel.rot(World::rot);

	direction dir = UP;

	if (vel) {

		if (fabsf(pvel.x) > fabsf(pvel.y)) {

			if (pvel.x < 0) dir = LEFT;
			else dir = RIGHT;

		} else {

			if (pvel.y < 0) dir = DOWN;
			else dir = UP;

		}

	} else dir = lastDir;

	Point tdim = {3, 4};
	Point frame;

	if (vel) frame.x = GFX::frame / 20;
	else frame.x = 1;

	frame.y = dir;

	GFX::drawSprite(tex, &pos, &dim, -World::rot, &tdim, &frame);

	lastDir = dir;

}
