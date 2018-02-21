#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "sign.hh"
#include "packet.hh"

extern "C" {

	bool init() {

		cputs(GREEN, "Loaded module: 'sign.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'sign.so'");

	}

}

void Sign::send(Client *client, char *string) {

	Packet packet;

	packet.size = strlen(string) + 2;
	packet.raw = (uint8_t*) malloc(packet.size + 1);

	packet.raw[0] = P_SIGN;
	strcpy((char*) packet.raw + 1, string);

	client->send(&packet);
	free(packet.raw);

}

void Sign::send(char *string) {

	Packet packet;

	packet.size = strlen(string) + 2;
	packet.raw = (uint8_t*) malloc(packet.size + 1);

	packet.raw[0] = P_SIGN;
	strcpy((char*) packet.raw + 1, string);

	Client::broadcast(&packet);
	free(packet.raw);

}

