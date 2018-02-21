#include <stdlib.h>
#include <time.h>

#include "console.hh"
#include "entity.hh"
#include "explosion.hh"
#include "point.hh"
#include "tile.hh"
#include "world.hh"

extern "C" {

	bool init() {

		cputs(GREEN, "Loaded module: 'explosion.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'explosion.so'");

	}

}

Explosion::Explosion(Point *pos, float radius, float duration) {

	type = "explosion";
	dim = radius * 2;
	this->duration = duration * 1000;

	this->pos = *pos;

	// 10 ticks per sec
	//interval = 100;

	add();

}

bool Explosion::tick(timespec *time) {

	if (timer >= duration) {

		delete this;
		return true;

	}

	Point *tiles;
	unsigned int size = boundWorld(&tiles);

	for (unsigned int i = 0; i < size; i++) {

		Tile *tile = World::getTile(&tiles[i]);
		if (tile->id != Tile::ROCK) World::setTile(&tiles[i], Tile::LAVA);

	}

	free(tiles);
	return false;

}
