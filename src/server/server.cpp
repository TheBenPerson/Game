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

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stddef.h>

#include "config/config.hpp"
#include "net/net.hpp"
#include "player/player.hpp"
#include "server.hpp"
#include "timing/timing.hpp"
#include "world/world.hpp"

namespace Server {

	bool fork;
	bool running;
	Config config;
	Timing::thread t;

	void cleanup() {

		Net::cleanup();
		World::cleanup();

	}

	bool init(uint16_t port) {

		config.add("timeout", (void*) 30);
		config.load("cfg/server.cfg");

		// long should be uint
		Player::timeout = (long) config.get("timeout")->val;

		if (!World::init())
			return false;

		if (!Net::init(port))
			return false;

		return true;

	}

	bool start(bool fork, uint16_t port) {

		Server::fork = fork;

		if (!init(port)) return false;

		running = true;

		if (fork) t = Timing::createThread(tickLoop, NULL);
		else tickLoop(NULL);

		return true;

	}

	void stop() {

		running = false;
		if (fork) Timing::waitFor(t);

		Net::cleanup();

	}

	void* tickLoop(void *) {

		sockaddr_in addr;
		Packet packet;

		while (running) {

			packet.raw = (uint8_t*) malloc(P_MAX_SIZE);

			Net::wait();
			Net::recv(&addr, &packet);

			// free extra memory
			packet.raw = (uint8_t*) realloc((uint8_t*) packet.raw, packet.size + 1);

			Player *player = Player::get(&addr);

			if (player) player->recv(&packet);
			else {

				char *name = (char*) packet.raw;
				name[packet.size] = '\0';

				in_addr rAddr;
				rAddr.s_addr = addr.sin_addr.s_addr;
				char *ip = inet_ntoa(rAddr);

				if (Player::get(name)) {

					free(packet.raw);

					char msg[] = "%A player with that usename is already connected";
					msg[0] = P_DENY;

					packet.size = strlen(msg + 1) + 1;
					packet.raw = (uint8_t*) msg;

					Net::send(&addr, &packet);

					fprintf(stderr, "%s was kicked upon entry: %s\n", ip, msg + 1);
					continue;

				}

				player = new Player(&addr, name);
				player->send(P_ACCEPT);

				printf("Player %s (%s) connected.\n", name, ip);

			}

		}

		return NULL;

	}

}
