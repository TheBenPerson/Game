#include <math.h>
#include <stdlib.h>
#include <string.h>

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
		this->vel *= 5;

		this->rot = rot;

	}

	add();

}

bool Fireball::tick(timespec *time) {

	bool hit = false;

	Point *tiles;
	unsigned int size = boundWorld(&tiles);

	for (unsigned int i = 0; i < size; i++) {

		Tile *tile = World::getTile(&tiles[i]);
		if (tile->id == Tile::ROCK) hit = true;

	}

	if (size) free(tiles);

	if (hit) {

		new Explosion(&pos);

		delete this;
		return true;

	}

	Entity::tick(time);
	return false;

}

void* Fireball::toNet(unsigned int *size) {

	typedef struct {

		uint8_t pid;

		UPacket upacket;
		float rot;

	} __attribute__((packed)) Data;

	*size = sizeof(Data) + strlen(type) + 1;
	Data *data = (Data*) malloc(*size);

	data->pid = P_GENT;
	data->upacket.id = id;

	data->upacket.dim = dim;
	data->upacket.pos = pos;
	data->upacket.vel = vel;

	data->rot = rot;
	strcpy(data->upacket.type, type);

	return data;

}
