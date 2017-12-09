/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.hpp"
#include "console.hpp"
#include "net.hpp"
#include "server.hpp"
#include "timing.hpp"
#include "world.hpp"

static Timing::thread t;
static void* spawn(void *);
static bool running = true;

extern "C" {

	char* depends[] = {

		"server.so",
		"world.so",
		"net.so",
		NULL

	};

	bool init() {

		Client::timeout = (uint16_t) Server::config.get("timeout")->val;
		t = Timing::createThread(spawn, NULL);

		cputs(GREEN, "Loaded module: 'sclient.so'");
		return true;

	}

	void cleanup() {

		running = false;
		Timing::waitFor(t);

		cputs(YELLOW, "Unloaded module: 'sclient.so'");

	}

}

void* spawn(void *) {

	sockaddr_in addr;
	Packet packet;

	while (running) {

		Net::wait();
		Net::recv(&addr, &packet);

		Client *client = Client::get(&addr);

		if (client) client->recv(&packet);
		else {

			char *name = (char*) packet.raw;
			name[packet.size] = '\0';

			in_addr rAddr;
			rAddr.s_addr = addr.sin_addr.s_addr;
			char *ip = inet_ntoa(rAddr);

			if (Client::get(name)) {

				free(packet.raw);

				char msg[] = "%A client with that usename is already connected\n";
				msg[0] = P_DENY;

				packet.size = strlen(msg + 1) + 1;
				packet.raw = (uint8_t*) msg;

				Net::send(&addr, &packet);

				fprintf(stderr, "%s was kicked upon entry: %s\n", ip, msg + 1);
				continue;

			}

			client = new Client(&addr, name);
			printf("Client %s (%s) connected.\n", name, ip);

		}

	}

	return NULL;

}

NodeList Client::clients;
uint Client::timeout;

Client* Client::get(sockaddr_in *addr) {

	Client *client;

	for (size_t i = 0; i < clients.len; i++) {

		client = (Client*) clients.get(i);

		if (client->addr.sin_addr.s_addr != addr->sin_addr.s_addr) continue;
		if (client->addr.sin_port != addr->sin_port) continue;

		return client;

	}

	return NULL;

}

Client* Client::get(char *name) {

	Client *client;

	for (size_t i = 0; i < clients.len; i++) {

		client = (Client*) clients.get(i);
		if (!strcmp(client->name, name)) return client;

	}

	return NULL;

}

void* Client::entry(void* arg) {

	Client *client = (Client*) arg;
	client->send(P_ACCEPT);

	Packet *packet;

	for (;;) {

		packet = client->recv();
		if (!packet) {

			printf("Client %s lost connection\n", client->name);

			delete client;
			return NULL;

		}

		switch (packet->id) {

			case P_GMAP: {

				printf("Sending map to %s...\n", client->name);

				Packet packet;
				packet.size = sizeof(World::Tile) * World::width * World::height;
				packet.raw = (uint8_t*) malloc(3 + packet.size);

				packet.raw[0] = P_SMAP;
				packet.raw[1] = World::width;
				packet.raw[2] = World::height;

				memcpy(packet.raw + 3, World::data, packet.size);
				packet.size += 2;

				client->send(&packet);
				free(packet.raw);

			} break;

		}

		free(packet->raw);

	}

	delete client;

}

Client::Client(sockaddr_in *addr, char *name) {

	memcpy(&this->addr, addr, sizeof(sockaddr_in));

	this->name = name;

	clients.add((void*) this);
	t = Timing::createThread(&Client::entry, (void*) this);

}

Client::~Client() {

	Client *client;

	for (size_t i = 0; i < clients.len; i++) {

		client = (Client*) clients.get(i);
		if (this == client) clients.rem(i);

	}

	free(name);

}

void Client::send(uint8_t id) {

	Packet packet;
	packet.id = id;
	packet.size = 1;

	send(&packet);

}

void Client::send(Packet *packet) {

	Net::send(&addr, packet);

}

void Client::recv(Packet *packet) {

	this->packet = packet;
	Timing::signal(&cond);

}

Packet* Client::recv() {

	// returns false if time ran out
	bool result = Timing::waitFor(&cond, timeout);
	if (!result) return NULL;

	return packet;

}
