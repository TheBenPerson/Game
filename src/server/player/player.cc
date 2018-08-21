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
#include "sign.hh"
#include "timing.hh"
#include "world.hh"

static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		Net::listeners.add((uintptr_t) &tickNet);
		return true;

	}

	void cleanup() {

		Net::listeners.rem((uintptr_t) &tickNet);

	}

}

Player::Player(char *name): Human(World::defaultWorld, false) {

	this->name = name;
	send();

}

bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		// todo: perhaps Entity should handle this?
		case P_IDNT: {

			Auth *auth = Auth::get(client);
			if (!auth) {

				client->send(P_DENY);
				break;

			}

			if (!auth->player) auth->player = new Player(client->name);

			uint8_t data[3];

			data[0] = P_IDNT;
			*((uint16_t*) (data + 1)) = auth->player->id;

			Packet packet;
			packet.size = 3;
			packet.raw = data;
			client->send(&packet);

			char *text =
			"Welcome to GAME: a game about nothing\n\n"

				"Press WASD to move around\n"
				"Hold [SECONDARY] to click on signs\n\n\n\n\n\n\t"

				"Press [PRIMARY] to continue...";

			Sign::send(client, text);

		} return true;

		case P_UPDP: {

			Player *player = Auth::get(client)->player;
			if (!player) return true;

			player->pos = Point(packet->data);
			player->vel = Point(packet->data + SIZE_TPOINT);

			Packet packet;
			player->toNet(&packet);

			for (unsigned int i = 0; i < Client::clients.size; i++) {

				Client *dest = (Client*) Client::clients[i];
				if (dest == client) continue;

				dest->send(&packet);

			}

			free(packet.raw);

		} break;

		case P_KICK:
		case P_DENY: {

			// todo: auth?

			Player *player = Auth::get(client)->player;
			if (player) delete player;

		} break;

		default: return false;

	}

	return true;

}
