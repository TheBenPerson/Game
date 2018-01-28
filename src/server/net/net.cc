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
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "console.hh"
#include "main.hh"
#include "net.hh"

static int sock;

extern "C" {

	char* net_deps[] = {

		// world has to be up before we start listening
		"world.so",
		NULL

	};

	bool init() {

		sock = socket(AF_INET, SOCK_DGRAM, 0);

		if (sock == -1) {

			ceprintf(RED, "Error loading module 'net.so': error creating socket: %s - exiting...\n", strerror(errno));
			return false;

		}

		sockaddr_in addr;
		memset(&addr, 0, sizeof(sockaddr_in));

		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(Game::port);

		if (bind(sock, (sockaddr*) &addr, sizeof(sockaddr_in)) == -1) {

			ceprintf(RED, "Error loading module 'net.so': error binding socket: %s - exiting...\n", strerror(errno));
			close(sock);

			return false;

		}

		cputs(GREEN, "Loaded module: 'net.so'");
		return true;

	}

	void cleanup() {

		Net::stop(); // in case client.so doesn't
		close(sock);

		cputs(YELLOW, "Unloaded module: 'net.so'");

	}

}

namespace Net {

	NodeList listeners;

	void stop() {

		shutdown(sock, SHUT_RDWR);

	}

	void send(sockaddr_in *addr, Packet *packet) {

		sendto(sock, packet->raw, packet->size, NULL, (sockaddr*) addr, sizeof(sockaddr_in));

	}

	bool recv(sockaddr_in *addr, Packet *packet) {

		pollfd p;
		p.fd = sock;
		p.events = POLLIN;
		p.revents = NULL;

		poll(&p, 1, -1);
		if (p.revents & POLLHUP) return false;

		ioctl(sock, FIONREAD, &packet->size);
		packet->raw = (uint8_t*) malloc(packet->size);

		socklen_t len = sizeof(sockaddr_in);
		recvfrom(sock, packet->raw, packet->size, NULL, (sockaddr*) addr, &len);

		packet->id = packet->raw[0];
		packet->data = packet->raw + 1;

		return true;

	}

}
