#include <math.h>
#include <stdint.h>
#include <string.h>

#include "entity.hh"
#include "gfx.hh"
#include "net.hh"
#include "packet.hh"
#include "point.hh"
#include "tiledef.hh"
#include "world.hh"

typedef struct {

	const char *name;
	void (*create)(uint8_t*);

} Entry;

static NodeList entries;

static bool tickNet(Packet *packet);
static void draw();

extern "C" {

	bool init() {

		Net::listeners.add((uintptr_t) &tickNet);
		World::listeners.add((uintptr_t) &draw);

		Net::send(P_GENT);
		return true;

	}

	void cleanup() {

		while (Entity::entities.size) {

			Entity *entity = (Entity*) Entity::entities[0];

			// destructor removes entity from list
			delete entity;

		}

		while (entries.size) {

			Entry *entry = (Entry*) entries[0];
			entries.rem((uintptr_t) entry);

			delete entry;

		}

		World::listeners.rem((uintptr_t) &draw);
		Net::listeners.rem((uintptr_t) &tickNet);

	}

}

// id
// type
// pos
// extra

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GENT: {

			if (packet->size < SIZE_TENTITY) break;

			unsigned int id = *((uint16_t*) packet->data);

			Entity *entity = Entity::get(id);
			if (entity)	{

				entity->unpack(packet->data + 12);
				break;

			}

			char type[11] = {0};
			strncpy(type, (char*) packet->data + 2, 10);

			for (unsigned int i = 0; i < entries.size; i++) {

				Entry *entry = (Entry*) entries[i];
				if (!strcmp(type, entry->name)) {

					entry->create(packet->data);
					break;

				}

			}

		} break;

		case P_DENT: {

			if (packet->size != 2) break;

			unsigned int id = *((uint16_t*) packet->data);
			delete Entity::get(id);

		} break;

		default: return false;

	}

	return true;

}

void draw() {

	for (unsigned int i = 0; i < Entity::entities.size; i++) {

		Entity *entity = (Entity*) Entity::entities[i];
		entity->draw();

	}

}

NodeList Entity::entities;

Entity* Entity::get(unsigned int id) {

	for (unsigned int i = 0; i < entities.size; i++) {

		Entity *entity = (Entity*) entities[i];
		if (entity->id == id) return entity;

	}

	return NULL;

}

void Entity::regEnt(const char *name, void (*create)(uint8_t*)) {

	Entry *entry = new Entry;
	entry->name = name;
	entry->create = create;

	entries.add((uintptr_t) entry);

}

Entity::Entity(uint8_t *data) {

	id = *((uint16_t*) data);
	unpack(data + 12);

	entities.add((uintptr_t) this);

}

Entity::~Entity() {

	entities.rem((uintptr_t) this);

}

void Entity::unpack(uint8_t *buff) {

	dim = Point(buff);
	buff += SIZE_TPOINT;

	pos = Point(buff);
	buff += SIZE_TPOINT;

	vel = Point(buff);

}

/*
 *
 * Updates entity position in sync with server
 * and does bounds checking
 *
 */

void Entity::draw() {

	// todo: change 60 to frame rate

	Point dpos = vel / 60.0f;
	pos += dpos;

	// some client side collision detection

	int dwidth = World::width / 2;
	int dheight = World::height / 2;

	Point ddim = dim / 2.0f;
	bool evenW = !(World::width % 2);
	bool evenH = !(World::height % 2);

	// if dimensions odd align to even ones
	int top = floor(pos.y + ddim.y + (!evenH * .5f));
	if (top < -dheight) return;
	if (top > (dheight - evenH)) top = dheight - evenH;

	int bottom = floor(pos.y - ddim.y + (!evenH * .5f));
	if (bottom > (dheight - evenH)) return;
	if (bottom < -dheight) bottom = -dheight;

	int left = floor(pos.x - ddim.x + (!evenW * .5f));
	if (left > (dwidth - evenW)) return;
	if (left < -dwidth) left = -dwidth;

	int right = floor(pos.x + ddim.x + (!evenW * .5f));
	if (right < -dwidth) return;
	if (right > (dwidth - evenW)) right = dwidth - evenW;

	// the +1 is to include the heighest value itself
	for (int y = bottom; y < top + 1; y++) {
	for (int x = left; x < right + 1; x++) {

		// exact center of tile
		Point tilePos = {x + (evenW * .5f), y + (evenH * .5f)};
		unsigned int index = World::getIndex(&tilePos);

		// don't collide with grass, etc.
		switch (World::tiles[index]) {

			case Tiledef::ROCK:
			case Tiledef::SIGN:
			break;

			default: continue;

		}

		float dx = pos.x - tilePos.x;
		float dy = pos.y - tilePos.y;

		bool colX = false;
		bool colY = false;

		// if collision on X axis
		if (fabsf(dx) > fabsf(dy)) {

			// check if the side of collision
			// is covered by a neighboring block

			Point point = tilePos;

			if (dx > 0) point.x++;
			else point.x--;

			index = World::getIndex(&point);
			if (World::tiles[index] != Tiledef::ROCK) colX = true;

		// if collision on Y axis
		} else if (fabsf(dx) < fabsf(dy)) {

			// check if the side of collision
			// is covered by a neighboring block

			Point point = tilePos;

			if (dy > 0) point.y++;
			else point.y--;

			index = World::getIndex(&point);
			if (World::tiles[index] != Tiledef::ROCK) colY = true;

		} else {

			// corner collision
			colX = true;
			colY = true;

		}

		// fix collision

		if (colX) {

			pos.x = tilePos.x;

			if (dx > 0) pos.x += .5f + ddim.x;
			else pos.x -= .5f + ddim.x;

		}

		if (colY) {

			pos.y = tilePos.y;

			if (dy > 0) pos.y += .5f + ddim.y;
			else pos.y -= .5f + ddim.y;

		}

	}}

}
