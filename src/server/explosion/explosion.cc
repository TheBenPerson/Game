#include <stdlib.h>
#include <time.h>

#include "entity.hh"
#include "explosion.hh"
#include "point.hh"
#include "tiledef.hh"
#include "world.hh"

extern "C" {

	bool init() {

		return true;

	}

	void cleanup() {}

}

Explosion::Explosion(World *world, Point *pos, float radius, float duration): Entity(world) {

	type = "explosion";
	dim = radius * 2;

	this->world = world;
	this->duration = duration * 1000;
	this->pos = *pos;

	// 10 ticks per sec
	//interval = 100;

	send();

}

bool Explosion::tick(unsigned int time) {

	if (timer >= duration) {

		delete this;
		return true;

	}

	Point *tiles;
	unsigned int size = boundWorld(&tiles);

	for (unsigned int i = 0; i < size; i++) {

		Tile *tile = world->getTile(&tiles[i]);
		if (tile->id == Tiledef::GRASS) world->setTile(&tiles[i], Tiledef::DIRT);

	}

	if (size) free(tiles);
	return false;

}
