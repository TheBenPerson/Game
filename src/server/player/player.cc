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
#include "human.hh"
#include "net.hh"
#include "packet.hh"
#include "timing.hh"
#include "world.hh"

static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		Net::listeners.add((intptr_t) &tickNet);

		cputs(GREEN, "Loaded module: 'player.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((intptr_t) &tickNet);
		cputs(YELLOW, "Unloaded module: 'player.so'");

	}

}

bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		case P_NEWP: {

			struct {

				uint8_t pid = P_IDNT;
				uint16_t id;

			} __attribute__((packed)) data;

			Entity *entity = Entity::get(client);

			if (entity) data.id = entity->id;
			else data.id = (new Human(client))->id;

			Packet packet;
			packet.size = sizeof(data);
			packet.raw = (uint8_t*) &data;
			client->send(&packet);

		} return true;

		case P_UPDP: {

			struct Data {

				__attribute__((packed)) Point pos;
				__attribute__((packed)) Point vel;

			} __attribute__((packed)) *data = (Data*) packet->data;

			Entity *entity = Entity::get(client);
			if (!entity) return true;

			entity->pos = data->pos;
			entity->vel = data->vel;

			// replaces entity update
			struct {

				uint8_t pid;
				Entity::UPacket packet;

			} __attribute__((packed)) dat;

			dat.pid = P_UENT;
			dat.packet.id = entity->id;

			dat.packet.dim = entity->dim;
			dat.packet.pos = entity->pos;
			dat.packet.vel = entity->vel;

			Packet packet;
			packet.raw = (uint8_t*) &dat;
			packet.size = sizeof(dat);

			for (unsigned int i = 0; i < Client::clients.size; i++) {

				Client *client = (Client*) Client::clients[i];
				if (client == entity->client) continue;

				client->send(&packet);

			}


		} break;

		case P_KICK:
		case P_DENY: {

			Entity *entity = Entity::get(client);
			if (entity) delete entity;

		} break;

		default: return false;

	}

	return true;

}
