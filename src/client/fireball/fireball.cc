#include <math.h>

#include "fireball.hh"
#include "gfx.hh"

static GFX::texture tex;

static void create(uint8_t *data);

extern "C" {

	bool init() {

		Entity::regEnt("fireball", &create);
		tex = GFX::loadTexture("fireball.png");

		return true;

	}

	void cleanup() {

		// todo: unreg ent?
		GFX::freeTexture(&tex);

	}

}

Fireball::Fireball(uint8_t *data): Entity(data) {

	// todo: check if there is an extra
	// if (extrasize != sizeof(float)) error();

	rot = *((float*) (data + SIZE_TENTITY));

}

void Fireball::draw() {

	Entity::draw();

	Point tdim = {8, 1};
	Point frame = {(float) (GFX::frame / 5), 0};
	GFX::drawSprite(tex, &pos, &dim, rot + M_PI_2, &tdim, &frame);

}

void create(uint8_t *data) {

	new Fireball(data);

}
