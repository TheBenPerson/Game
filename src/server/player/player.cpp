/*

Game Development Build
https://github.com/TheBenPerson/Game

Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/net.hpp"
#include "nodelist/nodelist.hpp"
#include "packet/packet.hpp"
#include "player.hpp"
#include "server.hpp"
#include "timing/timing.hpp"
#include "world/world.hpp"

NodeList Player::players;
uint Player::timeout;

Player* Player::get(sockaddr_in *addr) {

	Player *player;

	for (size_t i = 0; i < players.len; i++) {

		player = (Player*) players.get(i);

		if (player->addr.sin_addr.s_addr != addr->sin_addr.s_addr) continue;
		if (player->addr.sin_port != addr->sin_port) continue;

		return player;

	}

	return NULL;

}

Player* Player::get(char name[]) {

	Player *player;

	for (size_t i = 0; i < players.len; i++) {

		player = (Player*) players.get(i);
		if (!strcmp(player->name, name)) return player;

	}

	return NULL;

}

void* Player::entry(void* arg) {

	Player *player = (Player*) arg;
	Packet *packet;

	for (;;) {

		packet = player->recv();
		if (!packet) {

			printf("Player %s lost connection\n", player->name);

			delete player;
			return NULL;

		}

		switch (packet->raw[0]) {

			case P_GMAP: {

				printf("Sending map to %s...\n", player->name);

				Packet packet;
				packet.size = sizeof(World::Tile) * World::width * World::height;
				packet.raw = (uint8_t*) malloc(3 + packet.size);

				packet.raw[0] = P_ACCEPT;
				packet.raw[1] = World::width;
				packet.raw[2] = World::height;

				memcpy(packet.raw + 3, World::data, packet.size);
				packet.size += 2;

				player->send(&packet);
				free(packet.raw);

			} break;

		}

		free(packet->raw);

	}

	delete player;

}

Player::Player(sockaddr_in *addr, char *name) {

	memcpy(&this->addr, addr, sizeof(sockaddr_in));

	this->name = name;

	players.add((void*) this);
	t = Timing::createThread(&Player::entry, (void*) this);

}

Player::~Player() {

	Player *player;

	for (size_t i = 0; i < players.len; i++) {

		player = (Player*) players.get(i);
		if (this == player) players.rem(i);

	}

	free(name);

}

void Player::send(uint8_t id) {

	Packet packet;
	packet.raw[0] = id;
	packet.size = 1;

	send(&packet);

}

void Player::send(Packet *packet) {

	Net::send(&addr, packet);

}

void Player::recv(Packet *packet) {

	this->packet = packet;
	Timing::signal(&cond);

}

Packet* Player::recv() {

	// returns false if time ran out
	bool result = Timing::waitFor(&cond, timeout);
	if (!result) return NULL;

	return packet;

}
