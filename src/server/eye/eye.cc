#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "client.hh"
#include "console.hh"
#include "eye.hh"
#include "fireball.hh"
#include "packet.hh"
#include "world.hh"

extern "C" {

	bool init() {

		Point pos = {0, 0};
		new Eye(&pos);

		cputs(GREEN, "Loaded module: 'eye.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'eye.so'");

	}

}

Eye::Eye(Point *pos, Point *vel) {

	type = "eye";
	dim = {1, 1};

	this->pos = *pos;

	if (vel) this->vel = *vel;
	else this->vel = {0, 0};

	add();

}

bool Eye::tick(timespec *time) {

	bool change = false;

	if (timer >= 3000) {

		float angle = (rand() / (float) RAND_MAX) * M_PI * 2;
		new Fireball(&pos, angle - (M_PI / 9));
		new Fireball(&pos, angle);
		new Fireball(&pos, angle + (M_PI / 9));

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

		Tile *tile = World::getTile(&tiles[i]);
		switch (tile->id) {

			case Tile::ROCK:
			// other solids
			bounce = true;
			bp = &tiles[i];

			break;

			default: World::setTile(&tiles[i], Tile::ICE);
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
