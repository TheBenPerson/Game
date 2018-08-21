#include <math.h>
#include <stdlib.h>

#include "client.hh"
#include "eye.hh"
#include "fireball.hh"
#include "packet.hh"
#include "tiledef.hh"
#include "world.hh"

extern "C" {

	bool init() {

		Point point = {1, 0};
		new Eye(World::defaultWorld, &point, &point);

		return true;

	}

	void cleanup() {}

}

Eye::Eye(World *world, Point *pos, Point *vel): Entity(world) {

	type = "eye";
	dim = {1, 1};

	this->pos = *pos;

	if (vel) this->vel = *vel;
	else this->vel = {0, 0};

	send();

}

bool Eye::tick(unsigned int time) {

	bool change = false;

	if (timer >= 3000) {

		float angle = (rand() / (float) RAND_MAX) * M_PI * 2;
		new Fireball(world, &pos, angle - (M_PI / 9));
		new Fireball(world, &pos, angle);
		new Fireball(world, &pos, angle + (M_PI / 9));

		Point dvel = {cosf(angle), sinf(angle)};
		dvel /= 3;
		vel -= dvel;

		change = true;

		timer = 0;

	}

	bool bounce = false;
	Point *bp;

	Point *tiles;
	unsigned int size = boundWorld(&tiles);
	for (unsigned int i = 0; i < size; i++) {

		Tile *tile = world->getTile(&tiles[i]);
		switch (tile->id) {

			case Tiledef::ROCK:
			case Tiledef::SIGN:
			case Tiledef::CACTUS:
			// other solids
			bounce = true;
			bp = &tiles[i];

			break;

			default: world->setTile(&tiles[i], Tiledef::ICE);
			break;

		}

	}

	if (bounce) {

		float dx = abs(pos.x - bp->x);
		float dy = abs(pos.y - bp->y);

		if (dx > dy) vel.x *= -1;
		else vel.y *= -1;

	}

	if (size) free(tiles);

	if (bounce || change) update();
	Entity::tick(time);

	return false;

}
