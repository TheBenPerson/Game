#include <math.h>
#include <stdlib.h>

#include "client.hh"
#include "console.hh"
#include "explosion.hh"
#include "fireball.hh"
#include "tile.hh"
#include "world.hh"

extern "C" {

	bool init() {

		cputs(GREEN, "Loaded module: 'fireball.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'fireball.so'");

	}

}

Fireball::Fireball(Point *pos, float rot, Point *vel) {

	type = "fireball";
	dim = {1, 1};

	this->pos = *pos;

	if (vel) {

		this->vel = *vel;
		this->rot = atanf(vel->y / vel->x);

	} else {

		this->vel = {cosf(rot), sinf(rot)};
		this->rot = rot;

	}

	add();

}

bool Fireball::tick(timespec *time) {

	bool bounce = false;

	Point *tiles;
	unsigned int size = boundWorld(&tiles);

	for (unsigned int i = 0; i < size; i++) {

		Tile *tile = World::getTile(&tiles[i]);
		if (tile->id == Tile::ROCK) bounce = true;

	}

	free(tiles);

	if (bounce) {

		new Explosion(&pos);

		delete this;
		return true;

	}

	Entity::tick(time);
	return false;

}
