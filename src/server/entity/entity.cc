#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "client.hh"
#include "console.hh"
#include "entity.hh"
#include "net.hh"
#include "packet.hh"
#include "point.hh"
#include "timing.hh"
#include "world.hh"

static Timing::thread t;
static bool running = true;

static void* tmain(void*);
static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		t = Timing::createThread(&tmain, NULL);
		Net::listeners.add((intptr_t) &tickNet);

		cputs(GREEN, "Loaded module: 'entity.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((intptr_t) &tickNet);

		running = false;
		Timing::waitFor(t);

		while (Entity::entities.size) {

			Entity *entity = (Entity*) Entity::entities[0];
			delete entity;

		}

		cputs(YELLOW, "Unloaded module: 'entity.so'");

	}

}

void* tmain(void*) {

	while (running) {

		unsigned int toSleep = UINT_MAX;

		for (unsigned int i = 0; i < Entity::entities.size; i++) {

			Entity *entity = (Entity*) Entity::entities[i];

			timespec time;
			clock_gettime(CLOCK_MONOTONIC, &time);

			long dtime = time.tv_nsec - entity->time;

			// account for wraparound
			if (dtime < 0) dtime += 1000000000;

			// convert ns to ms
			dtime /= 1000000;

			if (dtime < entity->interval) dtime = entity->interval - dtime;
			else {

				bool killed = entity->tick(&time);
				if (killed) continue;

				entity->time = time.tv_nsec;
				entity->timer += entity->interval;

				dtime = 0;

			}

			if (dtime < toSleep) toSleep = dtime;

		}

		// TODO: account for time ticking other entities
		usleep(toSleep * 1000);

	}

	return NULL;

}

bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		case P_GENT:

			for (unsigned int i = 0; i < Entity::entities.size; i++) {

				Entity *entity = (Entity*) Entity::entities[i];

				Packet packet;
				entity->toNet(&packet);
				client->send(&packet);

				free(packet.raw);

			}

		break;

		default: return false;

	}

	return true;

}

NodeList Entity::entities;

void Entity::send() {

	// get next available id
	for (id = 0; get(id); id++) {}
	entities.add((intptr_t) this);

	Packet packet;
	toNet(&packet);

	Client::broadcast(&packet);

	free(packet.raw);

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

	int dwidth = World::width / 2;
	int dheight = World::height / 2;

	Point ddim = dim / 2.0f;
	bool evenW = !(World::width % 2);
	bool evenH = !(World::height % 2);

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

bool Entity::tick(timespec *time) {

	Point dpos = vel * (interval / 1000.0f);
	pos += dpos;

	return false;

}

Entity* Entity::get(unsigned int id) {

	for (unsigned int i = 0; i < entities.size; i++) {

		Entity *entity = (Entity*) entities[i];
		if (entity->id == id) return entity;

	}

	return NULL;

}

Entity* Entity::get(Client *client) {

	for (unsigned int i = 0; i < entities.size; i++) {

		Entity *entity = (Entity*) entities[i];
		if (entity->client == client) return entity;

	}

	return NULL;

}

Entity::Entity() {}
Entity::Entity(Client *client): client(client) {}

Entity::~Entity() {

	entities.rem((intptr_t) this);

	struct {

		uint8_t pid;
		uint16_t id;

	} __attribute__((packed)) data;

	data.pid = P_DENT;
	data.id = id;

	Packet packet;
	packet.raw = (uint8_t*) &data;
	packet.size = sizeof(data);

	Client::broadcast(&packet);

}

void Entity::toNet(Packet *packet) {

	struct Data {

		uint8_t pid;
		UPacket packet;

	} __attribute__((packed)) *data;
	packet->size = sizeof(Data) + strlen(type) + 1;
	data = (Data*) malloc(packet->size);

	data->pid = P_GENT;

	data->packet.id = id;
	data->packet.dim = dim;
	data->packet.pos = pos;
	data->packet.vel = vel;
	strcpy(data->packet.type, type);

	packet->raw = (uint8_t*) data;

}

void Entity::update() {

	struct {

		uint8_t pid;
		UPacket packet;

	} __attribute__((packed)) data;

	data.pid = P_UENT;
	data.packet.id = id;

	data.packet.dim = dim;
	data.packet.pos = pos;
	data.packet.vel = vel;

	Packet packet;
	packet.raw = (uint8_t*) &data;
	packet.size = sizeof(data);

	Client::broadcast(&packet);

}
