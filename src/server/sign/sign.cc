#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "packet.hh"
#include "sign.hh"
#include "tile.hh"
#include "tiledef.hh"

static Tile* newSign(void *text, bool freeSpecial);

extern "C" {

	bool init() {

		Tile::regTile(Tiledef::SIGN, &newSign);
		return true;

	}

	void cleanup() {

		// todo: unregister?

	}

}

Sign::Sign(char *text, bool freeStr): Tile(Tiledef::SIGN, text, freeStr) {}

void Sign::send(Client *client, char *str) {

	Packet packet;

	packet.size = strlen(str) + 2;
	packet.raw = (uint8_t*) malloc(packet.size);

	packet.raw[0] = P_SIGN;
	strcpy((char*) packet.raw + 1, str);

	client->send(&packet);
	free(packet.raw);

}

void Sign::send(char *str) {

	Packet packet;

	packet.size = strlen(str) + 2;
	packet.raw = (uint8_t*) malloc(packet.size + 1);

	packet.raw[0] = P_SIGN;
	strcpy((char*) packet.raw + 1, str);

	Client::broadcast(&packet);
	free(packet.raw);

}

void Sign::interact(Client *client) {

	Sign::send(client, (char*) special);

}

Tile* newSign(void *text, bool freeSpecial) {

	return new Sign((char*) text, freeSpecial);

}
