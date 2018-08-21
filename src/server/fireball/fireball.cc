#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "explosion.hh"
#include "fireball.hh"
#include "tiledef.hh"
#include "world.hh"

extern "C" {

	bool init() {

		return true;

	}

	void cleanup() {}

}

Fireball::Fireball(World *world, Point *pos, float rot, Point *vel): Entity(world) {

	type = "fireball";
	this->pos = *pos;

	if (vel) {

		this->vel = *vel;
		this->rot = atanf(vel->y / vel->x);

	} else {

		this->vel = {cosf(rot), sinf(rot)};
		this->vel *= 5;

		this->rot = rot;

	}

	send();

}

bool Fireball::tick(unsigned int time) {

	bool hit = false;

	Point *tiles;
	unsigned int size = boundWorld(&tiles);

	for (unsigned int i = 0; i < size; i++) {

		Tile *tile = world->getTile(&tiles[i]);
		if (tile->id == Tiledef::ROCK) hit = true;

	}

	if (size) free(tiles);

	if (hit) {

		new Explosion(world, &pos);

		delete this;
		return true;

	}

	Entity::tick(time);
	return false;

}

void Fireball::toNet(Packet *packet) {

	packet->size = 1 + SIZE_TENTITY + sizeof(float);
	packet->raw = (uint8_t*) malloc(packet->size);
	packet->raw[0] = P_GENT;
	pack(packet->raw + 1);

	*((float*) (packet->raw + 1 + SIZE_TENTITY)) = rot;

}
