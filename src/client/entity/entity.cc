#include <GL/gl.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "console.hh"
#include "entity.hh"
#include "gfx.hh"
#include "net.hh"
#include "packet.hh"
#include "point.hh"
#include "tiledef.hh"
#include "world.hh"

static bool tickNet(Packet *packet);
static void draw();

extern "C" {

	bool init() {

		Net::listeners.add((intptr_t) &tickNet);
		World::listeners.add((intptr_t) &draw);

		Net::send(P_GENT);

		cputs(GREEN, "Loaded module: 'entity.so'");
		return true;

	}

	void cleanup() {

		while (Entity::entities.size) {

			Entity *entity = (Entity*) Entity::entities[0];
			delete entity;

		}

		World::listeners.rem((intptr_t) &draw);
		Net::listeners.rem((intptr_t) &tickNet);

		cputs(YELLOW, "Unloaded module: 'entity.so'");

	}

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_UENT: {

			Entity::UPacket *upacket = (Entity::UPacket*) packet->data;

			Entity *entity = Entity::get(upacket->id); // todo: investicate crash here
			entity->update((uint8_t*) upacket);

		} break;

		case P_DENT: {

			Entity::UPacket *upacket = (Entity::UPacket*) packet->data;

			Entity *entity = Entity::get(upacket->id);
			delete entity;

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

bool Entity::verify(UPacket *packet, char *type) {

	return !strcmp(packet->type, type);

}

Entity::Entity(void *info) {

	UPacket *upacket = (UPacket*) info;

	id = upacket->id;
	update((uint8_t*) upacket);

	entities.add((intptr_t) this);

}

Entity::~Entity() {

	entities.rem((intptr_t) this);

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

	bool colX = false;
	bool colY = false;

	// the +1 is to include the heighest value itself
	for (int y = bottom; y < top + 1; y++) {
	for (int x = left; x < right + 1; x++) {

		Point point = {(float) x, (float) y};
		unsigned int index = World::getIndex(&point);

		// don't collide with grass, etc.
		if (World::tiles[index] != T_ROCK) continue;

		// convert block coords to world coords
		float dx = pos.x - (x + (evenW * .5f));
		float dy = pos.y - (y + (evenH * .5f));

		// if collision on X axis
		if (fabsf(dx) > fabsf(dy)) {

			// check if the side of collision
			// is covered by a neighboring block

			if (dx > 0) point.x++;
			else point.x--;

			index = World::getIndex(&point);
			if (World::tiles[index] != T_ROCK) colX = true;

		// if collision on Y axis
		} else if (fabsf(dx) < fabsf(dy)) {

			// check if the side of collision
			// is covered by a neighboring block

			if (dy > 0) point.y++;
			else point.y--;

			index = World::getIndex(&point);
			if (World::tiles[index] != T_ROCK) colY = true;

		} else {

			// on corner

		}

	}}

	if (colX) pos.x -= dpos.x;
	if (colY) pos.y -= dpos.y;

}

void Entity::update(uint8_t *info) {

	UPacket *upacket = (UPacket*) info;

	dim = upacket->dim;
	pos = upacket->pos;
	vel = upacket->vel;

}
