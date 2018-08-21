#include <stdint.h>

#include "client.hh"
#include "eye.hh"
#include "entity.hh"
#include "gfx.hh"
#include "world.hh"

static GFX::texture tex;

void callback(uint8_t *data);

extern "C" {

	bool init() {

		Entity::regEnt("eye", &callback);
		tex = GFX::loadTexture("eye.png");

		return true;

	}

	void cleanup() {

		// todo: unreg ent?

		GFX::freeTexture(&tex);

	}

}

Eye::Eye(uint8_t *data): Entity(data) {}

void Eye::draw() {

	Entity::draw();

	Point tdim = {16, 1};
	Point frame = {(float) (GFX::frame / 2), 0};

	GFX::drawSprite(tex, &pos, &dim, NULL, &tdim, &frame);

}

void callback(uint8_t *data) {

	new Eye(data);

}
