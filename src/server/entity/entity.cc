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
#include "timing.hh"
#include "world.hh"

static Timing::thread t;
static bool running = true;

static void* tmain(void*);
static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		t = Timing::createThread(&tmain, NULL);
		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'entity.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);

		running = false;
		Timing::waitFor(t);

		while (Entity::entities.len) {

			Entity *entity = (Entity*) Entity::entities.get(0);
			delete entity;

		}

		cputs(YELLOW, "Unloaded module: 'entity.so'");

	}

}

void* tmain(void*) {

	while (running) {

		unsigned int toSleep = UINT_MAX;

		for (unsigned int i = 0; i < Entity::entities.len; i++) {

			Entity *entity = (Entity*) Entity::entities.get(i);

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

			for (unsigned int i = 0; i < Entity::entities.len; i++) {

				Entity *entity = (Entity*) Entity::entities.get(i);

				Packet packet;
				packet.raw = (uint8_t*) entity->toNet(&packet.size);
				client->send(&packet);

				free(packet.raw);

			}

		break;

		default: return false;

	}

	return true;

}

NodeList Entity::entities;

void Entity::add() {

	for (id = 0; get(id); id++) {}
	entities.add((void*) this);

	Packet packet;
	packet.raw = (uint8_t*) toNet(&packet.size);

	Client::broadcast(&packet);
	free(packet.raw);

}

bool Entity::bound(Point *pos, Point *dim, float rot) {

	/*
	 *
	 * 1########0
	 * #        #
	 * # points #
	 * #        #
	 * 2########3
	 *
	 * get object's points
	 * from perspective of entity
	 *
	 * this is easier than rotating both
	 * because this way once one is transformed
	 * our points are known
	 *
	 */

	Point points[4];
	Point ddim = *dim / 2;

	points[0] = {ddim.x, ddim.y};
	points[0] -= this->pos;
	points[0].rot(rot);
	points[0] += *pos;
	points[0].rot(-this->rot);
	points[0] += this->pos;

	points[1] = {-ddim.x, ddim.y};
	points[1] -= this->pos;
	points[1].rot(rot);
	points[1] += *pos;
	points[1].rot(-this->rot);
	points[1] += this->pos;

	points[2] = {-ddim.x, -ddim.y};
	points[2] -= this->pos;
	points[2].rot(rot);
	points[2] += *pos;
	points[2].rot(-this->rot);
	points[2] += this->pos;

	points[3] = {ddim.x, -ddim.y};
	points[3] -= this->pos;
	points[3].rot(rot);
	points[3] += *pos;
	points[3].rot(-this->rot);
	points[3] += this->pos;

	/*
	 *
	 * next check for collision by
	 * checking all 4 sides of object
	 * against all sides of entity
	 * to see if any intersect
	 *
	 */

	bool result = true;
	for (unsigned int i = 0; i < 4; i++) {

		Point *p1 = points + i;
		Point *p2 = points + ((i + 1) % 4);

		ddim = this->dim / 2;

		// are all points inside *this?
		if (result) {

			if (!((p1->x >= (this->pos.x - ddim.x)) && (p1->x <= (this->pos.x + ddim.x)))) result = false;
			if (!((p1->y >= (this->pos.y - ddim.y)) && (p1->y <= (this->pos.y + ddim.y)))) result = false;

		}

		Point *left;
		Point *right;

		if (p1->x <= p2->x) {

			left = p1;
			right = p2;

		} else {

			left = p2;
			right = p1;

		}

		Point *top;
		Point *bottom;

		if (p1->y <= p2->y) {

			top = p2;
			bottom = p1;

		} else {

			top = p1;
			bottom = p2;

		}

		// check division by zero
		bool divz = !(right->x - left->x);

		float slope;
		if (!divz) slope = (right->y - left->y) / (right->x - left->x);
		float b = left->y - (slope * left->x);

		if ((left->x <= (this->pos.x - ddim.x)) && (right->x >= (this->pos.x - ddim.x))) {

			if (divz) return true;

			float y = (slope * (this->pos.x - ddim.x)) + b;
			if ((y >= (this->pos.y - ddim.y)) && (y <= (this->pos.y + ddim.y))) return true;

		}

		if ((left->x <= (this->pos.x + ddim.x)) && (right->x >= (this->pos.x + ddim.x))) {

			if (divz) return true;

			float y = (slope * (this->pos.x + ddim.x)) + b;
			if ((y >= (this->pos.y - ddim.y)) && (y <= (this->pos.y + ddim.y))) return true;

		}

		if ((bottom->y <= (this->pos.y - ddim.y)) && (top->y >= (this->pos.y - ddim.y))) {

			float x;

			if (divz) x = left->x;
			else x = ((this->pos.y - ddim.y) - b) / slope;

			if ((x >= (this->pos.x - ddim.x)) && (x <= (this->pos.x + ddim.x))) return true;

		}

		if ((bottom->y <= (this->pos.y + ddim.y)) && (top->y >= (this->pos.y + ddim.y))) {

			float x;

			if (divz) x = left->x;
			else x = ((this->pos.y + ddim.y) - b) / slope;

			if ((x >= (this->pos.x - ddim.x)) && (x <= (this->pos.x + ddim.x))) return true;

		}

	}

	return result;

}

unsigned int Entity::boundWorld(Point **tiles) {

	/*
	 *
	 * 1########0
	 * #        #
	 * # points #
	 * #        #
	 * 2########3
	 *
	 */

	// get points of entity's rectangle

	Point points[4];
	Point ddim = dim / 2;

	points[0] = {ddim.x, ddim.y};
	points[0].rot(rot);
	points[0] += pos;

	points[1] = {-ddim.x, ddim.y};
	points[1].rot(rot);
	points[1] += pos;

	points[2] = {-ddim.x, -ddim.y};
	points[2].rot(rot);
	points[2] += pos;

	points[3] = {ddim.x, -ddim.y};
	points[3].rot(rot);
	points[3] += pos;

	// find bounding box extrema
	// to narrow down results

	float top;
	top = fmaxf(points[0].y, points[1].y);
	top = fmaxf(top, points[2].y);
	top = fmaxf(top, points[3].y);

	// if dimensions odd align to even ones
	if (World::height % 2) top += .5f;
	top = floorf(top);

	// cap to world dimensions
	if (top > (World::height / 2)) top = World::height / 2;

	float bottom;
	bottom = fminf(points[0].y, points[1].y);
	bottom = fminf(bottom, points[2].y);
	bottom = fminf(bottom, points[3].y);

	if (World::height % 2) bottom += .5f;
	bottom = floorf(bottom);

	// if not casted to int interpreted as negative unsinged int
	if (bottom < -(((int) World::height) / 2)) bottom = -(((int) World::height) / 2);

	float left;
	left = fminf(points[0].x, points[1].x);
	left = fminf(left, points[2].x);
	left = fminf(left, points[3].x);

	if (World::width % 2) left += .5f;
	left = floorf(left);

	if (left < -(((int) World::width) / 2)) left = -(((int) World::width) / 2);

	float right;
	right = fmaxf(points[0].x, points[1].x);
	right = fmaxf(right, points[2].x);
	right = fmaxf(right, points[3].x);

	if (World::width % 2) right += .5f;
	right = floorf(right);

	if (right > (World::width / 2)) right = World::width / 2;

	// allocate maximum possible tiles
	// tiles is an array of values to be used to index World::tiles
	// eg. World::tiles[tile[foobar]]

	unsigned int width = (right - left) + 1;
	unsigned int height = (top - bottom) + 1;

	// + 1 for null terminator
	*tiles = (Point*) malloc((sizeof(Point) * width * height) + 1);
	unsigned int index = 0;

	for (int y = bottom; y < top + 1; y++) {
	for (int x = left; x < right + 1; x++) {

		// bound expects pos in center of object
		Point pos = {x, y};

		// if even add .5f
		pos.x +=  0.5f * !(World::width % 2);
		pos.y +=  0.5f * !(World::height % 2);

		Point dim = {1, 1};

		// perform more precise check
		if (bound(&pos, &dim, 0)) (*tiles)[index++] = pos;

	}}

	*tiles = (Point*) realloc(*tiles, sizeof(Point) * (++index));

	return index;

}

bool Entity::tick(timespec *time) {

	Point dpos = vel * (interval / 1000.0f);
	pos += dpos;

	return false;

}

Entity* Entity::get(unsigned int id) {

	for (unsigned int i = 0; i < entities.len; i++) {

		Entity *entity = (Entity*) entities.get(i);
		if (entity->id == id) return entity;

	}

	return NULL;

}

Entity::~Entity() {

	entities.rem((void*) this);

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

void* Entity::toNet(unsigned int *size) {

	typedef struct {

		uint8_t pid;
		uint16_t id;

		__attribute__((packed)) Point dim;
		__attribute__((packed)) Point pos;
		__attribute__((packed)) Point vel;

		float rot;
		bool onfire;
		char type[];

	} __attribute__((packed)) Data;

	*size = sizeof(Data) + strlen(type) + 1;
	Data *data = (Data*) malloc(*size);

	data->pid = P_GENT;
	data->id = id;

	data->dim = dim;
	data->pos = pos;
	data->vel = vel;
	data->rot = rot;
	data->onfire = onfire;
	strcpy(data->type, type);

	return data;

}

void Entity::update() {

	struct {

		uint8_t pid;
		uint16_t id;

		__attribute__((packed)) Point dim;
		__attribute__((packed)) Point pos;
		__attribute__((packed)) Point vel;

		float rot;
		bool onfire;

	} __attribute__((packed)) data;

	data.pid = P_UENT;
	data.id = id;

	data.dim = dim;
	data.pos = pos;
	data.vel = vel;
	data.rot = rot;
	data.onfire = onfire;

	Packet packet;
	packet.raw = (uint8_t*) &data;
	packet.size = sizeof(data);

	Client::broadcast(&packet);

}
