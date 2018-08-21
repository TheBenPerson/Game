#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "auth.hh"
#include "client.hh"
#include "entity.hh"
#include "net.hh"
#include "packet.hh"
#include "player.hh"
#include "point.hh"
#include "timing.hh"
#include "world.hh"

static Timing::Condition condition;
static Timing::thread t;
static bool running = true;

static void* tmain(void*);
static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		t = Timing::createThread(&tmain, NULL);
		Net::listeners.add((uintptr_t) &tickNet);

		return true;

	}

	void cleanup() {

		Net::listeners.rem((uintptr_t) &tickNet);

		running = false;
		Timing::signal(&condition);
		Timing::waitFor(t);

	}

}

void* tmain(void*) {

	while (running) {

		volatile unsigned int toSleep = UINT_MAX;

		for (unsigned int w = 0; w < World::worlds.size; w++) {

			World *world = (World*) World::worlds[w];

			for (unsigned int i = 0; i < world->entities.size; i++) {

				Entity *entity = (Entity*) world->entities[i];

				unsigned int now = Timing::getTime();
				long dtime = now - entity->time;

				if (dtime < entity->interval) dtime = entity->interval - dtime;
				else {

					bool killed = entity->tick(now);
					if (killed) continue;

					entity->time = now;
					entity->timer += entity->interval;

					dtime = 0;

				}

				if (dtime < toSleep) toSleep = dtime;

			}

		}

		// TODO: account for time ticking other entities
		Timing::waitFor(&condition, toSleep);

	}

	return NULL;

}

bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		case P_GENT: {

			NodeList *entities = &Auth::get(client)->world->entities;

			for (unsigned int i = 0; i < entities->size; i++) {

				Entity *entity = (Entity*) (*entities)[i];

				Packet packet;
				entity->toNet(&packet);
				client->send(&packet);

				free(packet.raw);

			}

		} break;

		default: return false;

	}

	return true;

}

bool Entity::bound(Point *pos, Point *dim) {

	Point points[4];
	Point ddim = *dim / 2.0f;

	points[0] = {-ddim.x, -ddim.y};
	points[0] += *pos;

	points[1] = {ddim.x, ddim.y};
	points[1] += *pos;

	ddim = this->pos / 2.0f;

	if (points[0].x > (this->pos.x + ddim.x)) return false;
	if (points[1].x < (this->pos.x - ddim.x)) return false;
	if (points[0].y > (this->pos.x + ddim.x)) return false;
	if (points[1].y < (this->pos.x - ddim.x)) return false;

	return true;

}

unsigned int Entity::boundWorld(Point **tiles) {

	int dwidth = world->width / 2;
	int dheight = world->height / 2;

	Point ddim = dim / 2.0f;
	bool evenW = !(world->width % 2);
	bool evenH = !(world->height % 2);

	// if dimensions odd align to even ones
	int top = floor(pos.y + ddim.y + (!evenH * .5f));
	if (top < -dheight) return false;
	if (top > (dheight - evenH)) top = dheight - evenH;

	int bottom = floor(pos.y - ddim.y + (!evenH * .5f));
	if (bottom > (dheight - evenH)) return false;
	if (bottom < -dheight) bottom = -dheight;

	int left = floor(pos.x - ddim.x + (!evenW * .5f));
	if (left > (dwidth - evenW)) return false;
	if (left < -dwidth) left = -dwidth;

	int right = floor(pos.x + ddim.x + (!evenW * .5f));
	if (right < -dwidth) return false;
	if (right > (dwidth - evenW)) right = dwidth - evenW;

	// tiles is an array of values to be used to index World::tiles
	// eg. World::tiles[tile[foobar]]

	// +1 is because the set {1 to 3} includes 3 numbers; not 2
	unsigned int width = (right - left) + 1;
	unsigned int height = (top - bottom) + 1;

	*tiles = (Point*) malloc(sizeof(Point) * (width * height));
	unsigned int index = 0;

	// the +1 is to include the heighest value itself
	for (int y = bottom; y < top + 1; y++) {
	for (int x = left; x < right + 1; x++) {

		(*tiles)[index++] = {(float) x + (evenW * .5f), (float) y + (evenH * .5f)};

	}}

	return index;

}

void Entity::pack(uint8_t *buff) {

	*((uint16_t*) buff) = id;
	buff += 2;

	strcpy((char*) buff, type);
	buff += 10;

	dim.pack(buff);
	buff += SIZE_TPOINT;

	pos.pack(buff);
	buff += SIZE_TPOINT;

	vel.pack(buff);

}

void Entity::send() {

	Packet packet;
	toNet(&packet);

	Client::broadcast(&packet, &world->clients);

	free(packet.raw);

}

void Entity::transfer(World *world) {

	this->world->entities.rem((uintptr_t) this);

	uint8_t data[3];
	data[0] = P_DENT;
	*((uint16_t*) (data + 1)) = id;

	Packet packet;
	packet.size = 3;
	packet.raw = data;

	Client::broadcast(&packet, &this->world->clients);

	world->entities.add((uintptr_t) this);
	this->world = world;

	send();

}

bool Entity::tick(unsigned int time) {

	Point dpos = vel * (interval / 1000.0f);
	pos += dpos;

	return false;

}

Entity* Entity::get(unsigned int id) {

	for (unsigned int i = 0; i < World::defaultWorld->entities.size; i++) {

		Entity *entity = (Entity*) World::defaultWorld->entities[i];
		if (entity->id == id) return entity;

	}

	return NULL;

}

Entity::Entity(World *world): world(world) {

	// get next available id
	for (id = 0; get(id); id++) {}
	world->entities.add((uintptr_t) this);

}

Entity::~Entity() {

	world->entities.rem((uintptr_t) this);

	uint8_t data[3];

	data[0] = P_DENT;
	*((uint16_t*)(data + 1)) = id;

	Packet packet;
	packet.raw = data;
	packet.size = 3;

	Client::broadcast(&packet, &world->clients);

}

void Entity::toNet(Packet *packet) {

	packet->size = SIZE_TENTITY + 1;
	packet->raw = (uint8_t*) malloc(SIZE_TENTITY + 1);

	packet->raw[0] = P_GENT;
	pack(packet->raw + 1);

}

void Entity::update() {

	Packet packet;
	toNet(&packet);

	Client::broadcast(&packet, &world->clients);

	free(packet.raw);

}
