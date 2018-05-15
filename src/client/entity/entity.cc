#include <GL/gl.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "console.hh"
#include "entity.hh"
#include "gfx.hh"
#include "net.hh"
#include "packet.hh"
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
			entity->update(upacket);

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
	update(upacket);

	entities.add((intptr_t) this);

}

Entity::~Entity() {

	entities.rem((intptr_t) this);

}

void Entity::update(void *info) {

	UPacket *upacket = (UPacket*) info;

	dim = upacket->dim;
	pos = upacket->pos;
	vel = upacket->vel;

}
