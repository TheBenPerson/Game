#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gfx.hh"
#include "human.hh"
#include "world.hh"

GFX::texture tex;

static void create(uint8_t *data);

extern "C" {

	bool init() {

		Entity::regEnt("human", &create);

		tex = GFX::loadTexture("villager_f.png");
		return true;

	}

	void cleanup() {

		// todo: unreg ent?
		GFX::freeTexture(&tex);

	}

}

Human::Human(uint8_t *data): Entity(data) {

	// todo: unsafe?
	name = strdup((char*) data + SIZE_TENTITY);

}

Human::~Human() {

	free(name);

}

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

	if (vel) frame.x = GFX::frame / 10;
	else frame.x = 1;

	frame.y = dir;

	GFX::drawSprite(tex, &pos, &dim, -World::rot, &tdim, &frame);

	Point text = {0, dim.y / 2};
	text.y += .2f;

	text.rot(-World::rot);
	text += pos;

	GFX::drawText(name, &text, .4f, true, -World::rot);

	lastDir = dir;

}

void create(uint8_t *data) {

	new Human(data);

}
