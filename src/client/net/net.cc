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
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.hh"
#include "console.hh"
#include "main.hh"
#include "net.hh"
#include "timing.hh"

static Config *config;

static Timing::thread t;
static bool running = false;
static int dtimeout;
static int sock;

// todo: maybe not static

static char *name;
static char *host;
static uint16_t port;

static bool recv(Packet *packet) {

	static bool timed = false;

	pollfd p;
	p.fd = sock;
	p.events = POLLIN;
	p.revents = NULL;

	int result = poll(&p, 1, dtimeout);
	if (p.revents & POLLHUP) return false;

	if (!result) {

		if (timed) return false;

		Net::send(P_POKE);
		timed = true;

		if (!recv(packet)) return false;
		free(packet->raw);

		return recv(packet);

	}

	ioctl(sock, FIONREAD, &packet->size);
	packet->raw = (uint8_t*) malloc(packet->size);
	recv(sock, packet->raw, packet->size, NULL);

	packet->id = packet->raw[0];
	packet->size--;
	packet->data = packet->raw + 1;

	timed = false;
	return true;

}

static void* tmain(void*) {

	running = true;

	while (running) {

		Packet packet;
		if (!recv(&packet)) {

			if (running) {

				ceprintf(RED, "Disconnected: Server not responding\n");
				close(sock);

			} else puts("Disconnected");
			return NULL;

		}

		switch (packet.raw[0]) {

			case P_POKE:

				Net::send(P_ACCEPT);

			break;

			case P_DENY:

				packet.data[packet.size - 1] = '\0';
				printf("Disconnected (%s)\n", packet.data);

				// in case cleanup isn't called in time to set running to false
				running = false;
				Game::stop();

			break;

			default:

				for (unsigned int i = 0; i < Net::listeners.size; i++) {

					// todo: maybe Client:: instead?
					bool (*callback)(Packet*) = (bool (*)(Packet *)) Net::listeners[i];
					if (callback(&packet)) break;

				}

		}

		free(packet.raw);

	}

	return NULL;

}

extern "C" {

	bool init() {

		Config::Option options[] = {

			STRING("name", "John_Doe"),
			STRING("host", "localhost"),
			INT("timeout", 30),
			INT("port", 1270),
			END

		};

		config = new Config("cfg/client/net.cfg", options);

		name = config->getStr("name");
		host = config->getStr("host");

		dtimeout = config->getInt("timeout");
		dtimeout *= 1000;
		dtimeout /= 2; // todo: might be zero - tell user

		port = config->getInt("port");

		sock = socket(AF_INET, SOCK_DGRAM, 0);

		if (sock == -1) {

			ceprintf(RED, "Error creating socket: %s", strerror(errno)); // todo: make one simpler function
			return false;

		}

		addrinfo hints, *info;

		memset(&hints, NULL, sizeof(addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_protocol = SOCK_DGRAM;

		int result = getaddrinfo(host, NULL, &hints, &info);
		if (result) {

			const char *err;

			if (result == EAI_SYSTEM) err = strerror(errno);
			else err = gai_strerror(result);

			ceprintf(RED, "Error finding host '%s': %s\n", host, err);

			close(sock);
			return false;

		}

		((sockaddr_in*) info->ai_addr)->sin_port = htons(port);
		result = connect(sock, info->ai_addr, sizeof(sockaddr_in));
		freeaddrinfo(info);

		if (result == -1) { // todo: is this necessary?

			ceprintf(RED, "Error connecting: %s\n", strerror(errno));

			close(sock);
			return false;

		}

		Packet packet;
		packet.raw = (uint8_t*) name;
		packet.size = strlen(name) + 1;

		Net::send(&packet);
		if (!recv(&packet)) {

			ceputs(RED, "Connection failed: request timed out");
			close(sock);

			return false;

		}

		if (packet.raw[0] == P_DENY) {

			packet.raw[packet.size - 1] = '\0';
			ceprintf(RED, "Connection failed: Access denied (%s)\n", packet.raw + 1);

			free(packet.raw);
			close(sock);

			return false;

		} else if (packet.raw[0] != P_ACCEPT) {

			free(packet.raw);
			close(sock);

			ceputs(RED, "Connection failed: Invalid response");
			return false;

		}

		free(packet.raw);
		t = Timing::createThread(&tmain, NULL);

		printf("Connected to %s:%i\n", host, port);
		return true;

	}

	void cleanup() {

		running = false; // todo: remove maybe?
		Net::send(P_DENY);

		shutdown(sock, SHUT_RDWR);
		Timing::waitFor(t);

		close(sock);
		delete config;

	}

}

namespace Net {

	NodeList listeners;

	void send(uint8_t id) {

		Packet packet;
		packet.raw = &id;
		packet.size = 1;

		send(&packet);

	}

	void send(Packet *packet) {

		::send(sock, packet->raw, packet->size, NULL);

	}

}
