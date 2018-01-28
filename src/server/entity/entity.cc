#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "entity.hh"
#include "net.hh"
#include "packet.hh"
#include "timing.hh"

static Timing::thread t;
static bool running = true;

static void* tmain(void*);
static bool tickNet(Packet *packet, Client *client);

extern "C" {

	char* entity_deps[] = {

		"net.so",
		"world.so",
		NULL

	};

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

	for (unsigned int i = 0; i < Entity::entities.len; i++) {

		Entity *entity = (Entity*) Entity::entities.get(i);
		entity->tick();

	}

	// todo: sleep here

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

		/*case P_NENT:

			Entity::create(packet->data[0])

		break

		case P_DENT:

		break;

		case P_EUPD:


		break;*/

		default: return false;

	}

	return true;

}

NodeList Entity::entities;

Entity* Entity::get(unsigned int id) {

	for (unsigned int i = 0; i < entities.len; i++) {

		Entity *entity = (Entity*) entities.get(i);
		if (entity->id == id) return entity;

	}

	return NULL;

}

Entity::Entity(Point pos, Point vel): pos(pos), vel(vel) {

	entities.add((void*) this);

}

Entity::~Entity() {

	entities.rem((void*) this);

	// todo: send P_DENT to clients

}

void* Entity::toNet(unsigned int *size) {

	typedef struct {

		uint8_t pid;
		uint16_t id;

		__attribute__((packed)) Point pos;
		__attribute__((packed)) Point vel;

		char type[];

	} __attribute__((packed)) Data;

	char *type = getType();
	*size = sizeof(Data) + strlen(type) + 1;
	Data *data = (Data*) malloc(*size);

	data->pid = P_NENT;
	data->id = id;
	data->pos = pos;
	data->vel = vel;
	strcpy(data->type, type);

	return data;

}
