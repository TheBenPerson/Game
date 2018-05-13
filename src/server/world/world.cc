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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "net.hh"
#include "packet.hh"
#include "point.hh"
#include "server.hh"
#include "tiledef.hh"
#include "world.hh"

static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		Server::config.set("world.map", (void*) "default.map");
		Server::config.load("world.cfg");

		char *map = (char*) Server::config.get("world.map")->val;

		// todo: if failed call cleanup
		bool result = World::loadMap(map);
		if (!result) return false;

		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'world.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);

		for (unsigned int i = 0; i < (World::width * World::height); i++)
			delete World::tiles[i];

		free(World::tiles);

		cputs(YELLOW, "Unloaded module: 'world.so'");

	}

}

namespace World {

	Tile **tiles = NULL; // should be mutex locked...
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

		int result = fscanf(file, "%ix%i\n", &width, &height);
		if (result < 1 || result == EOF) {

			fclose(file);

			ceputs(RED, "Error loading world");
			return false;

		}

		unsigned int size = width * height;

		if (tiles) {

			for (unsigned int i = 0; i < size; i++)
				delete tiles[i];

			free(tiles);

		}

		tiles = (Tile**) malloc(sizeof(Tile*) * size);

		for (int row = height - 1; row != -1; row--) {
		for (unsigned int col = 0; col < width; col++) {

			unsigned int id;
			int result = fscanf(file, "%i,", &id);
			if (result == EOF) {

				fclose(file);

				ceputs(RED, "Error loading world");
				return false;

			}

			tiles[(row * width) + col] = Tile::newTile(id);

		}}

		// dummy to read newline
		fgetc(file);

		for (unsigned int i = 0; i < size; i++) {

			if (tiles[i]->id == T_SIGN) {

				size_t dummy = NULL;
				ssize_t result = getline((char**) &tiles[i]->special, &dummy, file);
				if (result == -1) {

					fclose(file);

					ceputs(RED, "Error loading world");
					return false;

				}

				tiles[i]->freeSpecial = true;

			}

		}

		fclose(file);

		printf("Loaded map '%s' (%ix%i)\n", name, width, height);
		return true;

	}

	Tile* getTile(Point *pos) {

		unsigned int x = pos->x + (width / 2);
		unsigned int y = pos->y + (height / 2);

		return tiles[(y * width) + x];

	}

	void setTile(Point *pos, uint8_t id) {

		unsigned int x = pos->x + (width / 2);
		unsigned int y = pos->y + (height / 2);

		unsigned int index = (y * width) + x;

		if (tiles[index]->id == id) return;
		tiles[index]->id = id;

		struct {

			uint8_t id = P_SBLK;
			uint16_t index;
			uint8_t type;

		} __attribute__((packed)) data;

		// workaround for same name glitch thingy
		data.index = index;
		data.type = id;

		Packet packet;
		packet.raw = (uint8_t*) &data;
		packet.size = sizeof(data);

		Client::broadcast(&packet);

	}

}


bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		case P_INTR: {

			Tile *tile = World::tiles[*((uint16_t*) packet->data)];
			tile->interact(client);

		} break;

		case P_GMAP: {

			printf("Sending map to %s...\n", client->name);

			Packet packet;
			packet.size = 3 + (World::width * World::height);
			packet.raw = (uint8_t*) malloc(packet.size);

			packet.raw[0] = P_GMAP;
			packet.raw[1] = World::width;
			packet.raw[2] = World::height;

			for (unsigned int i = 3; i < packet.size; i++)
				packet.raw[i] = World::tiles[i - 3]->id;

			client->send(&packet);
			free(packet.raw);

		} break;

		case P_SBLK: {

			struct Data {

				uint16_t index;
				uint8_t id;

			} __attribute__((packed)) *data = (Data*) packet->data;

			unsigned int index = data->index;
			World::tiles[index]->id = data->id;

			for (unsigned int i = 0; i < Client::clients.size; i++) {

				Client *dest = (Client*) Client::clients[i];
				if (dest == client) continue;

				// relay packet
				dest->send(packet);

			}

		} break;

		default: return false;

	}

	return true;

}
