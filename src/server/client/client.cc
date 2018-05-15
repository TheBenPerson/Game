/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2018 Ben Stockett <thebenstockett@gmail.com>
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

#include "client.hh"
#include "console.hh"
#include "net.hh"
#include "server.hh"
#include "timing.hh"

static Timing::thread t;
static void* spawn(void *);
static unsigned int dtimeout;

extern "C" {

	bool init() {

		Server::config.set("client.timeout", 30);
		Server::config.load("client.cfg");
		dtimeout = (unsigned int) Server::config.get("client.timeout")->val;
		dtimeout /= 2; // could be zero - tell user

		t = Timing::createThread(spawn, NULL);

		cputs(GREEN, "Loaded module: 'client.so'");
		return true;

	}

	void cleanup() {

		while (Client::clients.size) {

			Client *client = (Client*) Client::clients[0];
			client->kick("Server shut down");

		}

		Net::stop();
		Timing::waitFor(t);

		cputs(YELLOW, "Unloaded module: 'client.so'");

	}

}

void* spawn(void *) {

	sockaddr_in addr;
	Packet packet;

	for (;;) {

		bool result = Net::recv(&addr, &packet);
		if (!result) return NULL;

		Client *client = Client::get(&addr);

		if (client) client->recv(&packet);
		else {

			char *name = (char*) packet.raw;
			name[packet.size - 1] = '\0';

			in_addr rAddr;
			rAddr.s_addr = addr.sin_addr.s_addr;
			char *ip = inet_ntoa(rAddr);

			if (Client::get(name)) {

				free(packet.raw);

				char msg[] = "%A client with that username is already connected\n";
				msg[0] = P_DENY;

				packet.size = strlen(msg + 1) + 1;
				packet.raw = (uint8_t*) msg;

				Net::send(&addr, &packet);

				fprintf(stderr, "%s was kicked upon entry: %s", ip, msg + 1);
				continue;

			}

			client = new Client(&addr, name);
			printf("%s (%s) connected.\n", name, ip);

		}

	}

	return NULL;

}

NodeList Client::clients;
uint Client::timeout;

Client* Client::get(sockaddr_in *addr) {

	Client *client;

	for (size_t i = 0; i < clients.size; i++) {

		client = (Client*) clients[i];

		if (client->addr.sin_addr.s_addr != addr->sin_addr.s_addr) continue;
		if (client->addr.sin_port != addr->sin_port) continue;

		return client;

	}

	return NULL;

}

Client* Client::get(char *name) {

	Client *client;

	for (size_t i = 0; i < clients.size; i++) {

		client = (Client*) clients[i];
		if (!strcmp(client->name, name)) return client;

	}

	return NULL;

}

void* Client::entry(void* arg) {

	Client *client = (Client*) arg;
	client->send(P_ACCEPT);

	while (client->running) {

		Packet *packet = client->recv();
		if (!packet) {

			if (client->running) {

				printf("%s lost connection\n", client->name);
				client->kick("Timed out");

			}

			break;

		}

		switch (packet->id) {

			case P_POKE: client->send(P_ACCEPT);
			break;

			case P_DENY: printf("%s disconnected\n", client->name);
			case P_KICK: client->running = false;
			default:

				for (unsigned int i = 0; i < Net::listeners.size; i++) {

					// todo: maybe Client:: instead?
					bool (*callback)(Packet*, Client*) = (bool (*)(Packet *, Client*)) Net::listeners[i];
					if (callback(packet, client)) break;

				}

		}

		// todo: don't free dummy packet
		free(packet->raw);

	}

	delete client;
	return NULL;

}

void Client::broadcast(Packet *packet) {

	for (unsigned int i = 0; i < clients.size; i++) {

		Client *client = (Client*) clients[i];
		client->send(packet);

	}

}

Client::Client(sockaddr_in *addr, char *name) {

	memcpy(&this->addr, addr, sizeof(sockaddr_in));

	this->name = name;

	clients.add((intptr_t) this);
	t = Timing::createThread(&Client::entry, (void*) this);

}

Client::~Client() {

	clients.rem((intptr_t) this);
	free(name);

}

void Client::send(uint8_t id) {

	Packet packet;
	packet.raw = &id;
	packet.size = 1;

	send(&packet);

}

void Client::send(Packet *packet) {

	Net::send(&addr, packet);

}

void Client::recv(Packet *packet) {

	// todo: if busy put on a fifo?

	this->packet = packet;
	Timing::signal(&cond);

}

Packet* Client::recv() {

	// returns false if time ran out
	bool result = Timing::waitFor(&cond, dtimeout);
	if (result) return packet;

	send(P_POKE);
	result = Timing::waitFor(&cond, dtimeout);

	if (result) {

		free(packet->raw);
		return recv();

	} else return NULL;

}

void Client::kick(char *reason) {

	running = false;

	Packet packet;
	packet.size = strlen(reason) + 2;
	packet.raw = (uint8_t*) malloc(packet.size);
	packet.raw[0] = P_KICK;
	strcpy((char*) packet.raw + 1, reason);

	send(&packet);
	free(packet.raw);

	printf("Kicked %s (%s)\n", name, reason);

	uint8_t id = P_KICK;
	packet.raw = &id;
	packet.size = 1;
	recv(&packet); // send dummy packet to exit recv

	Timing::waitFor(this->t);

}
