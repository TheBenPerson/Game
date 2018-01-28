#include "console.hh"
#include "entity.hh"
#include "net.hh"
#include "packet.hh"
#include "world.hh"

static bool tickNet(Packet *packet);
static void draw();

extern "C" {

	char* entity_deps[] = {

		"net.so",
		"world.so",
		NULL

	};

	bool init() {

		Net::listeners.add((void*) &tickNet);
		World::listeners.add((void*) &draw);

		Net::send(P_GENT);

		cputs(GREEN, "Loaded module: 'entity.so'");
		return true;

	}

	void cleanup() {

		while (Entity::entities.len) {

			Entity *entity = (Entity*) Entity::entities.get(0);
			delete entity;

		}

		World::listeners.rem((void*) &draw);
		Net::listeners.rem((void*) &tickNet);

		cputs(YELLOW, "Unloaded module: 'entity.so'");

	}

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_UENT:

			Entity *entity = Entity::get(packet.raw[1]);
			entity->update(packet->raw + 2);

		break;

		default: return false;

	}

	return true;

}

void draw() {

	for (unsigned int i = 0; i < Entity::entities.len; i++) {

		Entity *entity = (Entity*) Entity::entities.get(i);
		entity->draw();

	}

}

NodeList Entity::entities;

Entity* Entity::get(unsigned int id) {

	for (unsigned int i = 0; i < entities.len; i++) {

		Entity *entity = (Entity*) entities.get(i);
		if (entity->id == id) return entity;

	}

	return NULL;

}

Entity::Entity(unsigned int id, Point pos, Point vel): id(id), pos(pos), vel(vel) {

	// todo: play sounds
	entities.add((void*) this);

}

Entity::~Entity() {

	entities.rem((void*) this);

}

void Entity::update(void *raw) {

	typedef struct {

		Point pos;
		Point vel;

	} __attribute__((packed)) Data;

	Data *data = (Data*) raw;

	pos = data->pos;
	vel = data->vel;

}
