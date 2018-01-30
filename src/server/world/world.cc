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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "net.hh"
#include "packet.hh"
#include "point.hh"
#include "world.hh"

static bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		case P_GMAP: {

			printf("Sending map to %s...\n", client->name);

			Packet packet;
			packet.size = 3 + (sizeof(Tile) * World::width * World::height);
			packet.raw = (uint8_t*) malloc(packet.size);

			packet.raw[0] = P_GMAP;
			packet.raw[1] = World::width;
			packet.raw[2] = World::height;

			memcpy(packet.raw + 3, World::tiles, packet.size - 3);

			client->send(&packet);
			free(packet.raw);

		} break;

		case P_SBLK: {

			unsigned int index = (packet->data[1] * World::width) + packet->data[0];
			World::tiles[index].id = (Tile::type) packet->data[2];

			for (unsigned int i = 0; i < Client::clients.len; i++) {

				Client *dest = (Client*) Client::clients.get(i);
				if (dest == client) continue;

				// relay packet
				dest->send(packet);

			}

		} break;

		default: return false;

	}

	return true;

}

extern "C" {

	bool init() {

		bool result = World::loadMap("main.map");
		if (!result) return false;

		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'world.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);

		free(World::tiles);
		cputs(YELLOW, "Unloaded module: 'world.so'");

	}

}

namespace World {

	Tile *tiles = NULL; // should be mutex locked...
	unsigned int width;
	unsigned int height;

	bool loadMap(char *name) {

		char *path = (char*) malloc(8 + strlen(name) + 1);
		sprintf(path, "res/map/%s", name);

		FILE *file = fopen(path, "r");
		free(path);

		if (!file) {

			ceprintf(RED, "Error opening file '%s': %s\n", name, strerror(errno));
			return false;

		}

		char buf[4];
		buf[0] = '\0';
		buf[1] = '\0';
		buf[2] = '\0';
		buf[3] = '\0';

		char c = fgetc(file);
		if ((c >= '0') && (c <= '9')) {

			buf[0] = c;

			c = fgetc(file);
			if ((c >= '0') && (c <= '9')) {

				buf[1] = c;

				c = fgetc(file);
				if ((c >= '0') && (c <= '9')) buf[2] = c;

			}

			width = atoi(buf);

		} else {

			fclose(file);

			ceprintf(RED, "Error parsing map '%s'\n", path);
			return false;

		}

		buf[0] = '\0';
		buf[1] = '\0';
		buf[2] = '\0';

		c = fgetc(file);
		if ((c >= '0') && (c <= '9')) {

			buf[0] = c;

			c = fgetc(file);
			if ((c >= '0') && (c <= '9')) {

				buf[1] = c;

				c = fgetc(file);
				if ((c >= '0') && (c <= '9')) buf[2] = c;

			}

			height = atoi(buf);

		} else {

			fclose(file);

			ceprintf(RED, "Error parsing map '%s'\n", path);
			return false;

		}

		unsigned int size = width * height;

		if (tiles) free(tiles);
		tiles = (Tile*) malloc(sizeof(Tile) * size);

		for (unsigned int i = 0; i < size; i++) {

			for (;;) {

				char buf[3];
				buf[1] = '\0';
				buf[2] = '\0';

				char c = fgetc(file);
				if ((c >= '0') && (c <= '9')) {

					buf[0] = c;

					c = fgetc(file);
					if ((c >= 0x30) && (c <= 0x39)) buf[1] = c;

					tiles[i].id = (Tile::type) atoi(buf);
					break;

				}

			}

		}

		printf("Loaded map '%s' (%ix%i)\n", name, width, height);
		return true;

	}

	void setTile(Point *pos, Tile::type id) {

		unsigned int x = pos->x + (width / 2.0f);
		unsigned int y = (height / 2.0f) - pos->y;

		unsigned int index = (y * width) + x;

		if (tiles[index].id == id) return;

		tiles[index].id = id;

		Packet packet;
		uint8_t data[] = { P_SBLK, x, y, id };

		packet.raw = data;
		packet.size = 4;

		Client::broadcast(&packet);

	}

}
