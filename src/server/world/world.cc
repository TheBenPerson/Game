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

#include "auth.hh"
#include "client.hh"
#include "console.hh"
#include "config.hh"
#include "entity.hh"
#include "net.hh"
#include "packet.hh"
#include "player.hh"
#include "point.hh"
#include "sign.hh"
#include "string.hh"
#include "tiledef.hh"
#include "world.hh"

World *World::defaultWorld = NULL;
NodeList World::worlds;

static int parseWorld(void *, void *, const char *value, void *result);
static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		// todo: if failed call cleanup
		World::defaultWorld = World::newWorld("main.map");
		if (!World::defaultWorld) return false;

		Net::listeners.add((uintptr_t) &tickNet);
		return true;

	}

	void cleanup() {

		Net::listeners.rem((uintptr_t) &tickNet);

		while (World::worlds.size)
			delete (World*) World::worlds[0];

	}

}

World* World::get(char *name) {

	for (unsigned int i = 0; i < worlds.size; i++) {

		World *world = (World*) worlds[i];
		if (!strcmp(world->name, name)) return world;

	}

	return NULL;

}

World* World::newWorld(char *name) {

	STRING_CAT2(path, "res/map/", name);

	Config::Option options[] = {

		INT("width", NULL),
		INT("height", NULL),
		CUSTOM("tiles", NULL, &parseWorld),
		STRING_LIST("specials", NULL),
		END

	};

	bool result;
	Config config(path, options, &result);

	if (!result) {

		ceprintf(RED, "Error loading world '%s': could not parse file\n", name);
		return NULL;

	}

	unsigned int width = config.getInt("width");
	unsigned int height = config.getInt("height");

	unsigned int size = width * height;

	if (size != config.getSize("tiles")) {

		ceprintf(RED, "Error loading world '%s': invalid dimensions\n", name);
		return NULL;

	}

	Tile **tiles = (Tile**) malloc(sizeof(Tile*) * size);

	unsigned int index = 0;
	unsigned int sindex = 0;

	for (int row = height - 1; row != -1; row--) {
	for (unsigned int col = 0; col < width; col++) {

		int id = config.getInt("tiles", index++);

		unsigned int i = (row * width) + col;
		tiles[i] = Tile::newTile(id);

		if (tiles[i]->id == Tiledef::SIGN || tiles[i]->id == Tiledef::DOOR) {

			size_t dummy = NULL;
			tiles[i]->special = strdup(config.getStr("specials", sindex++));
			tiles[i]->freeSpecial = true;

		}

	}}

	World *world = new World;
	world->name = name;
	world->width = width;
	world->height = height;
	world->tiles = tiles;

	worlds.add((uintptr_t) world);

	printf("Loaded map '%s' (%ix%i)\n", name, width, height);
	return world;

}

void World::sendWorld(Client *client) {

	printf("Sending map to %s...\n", client->name);

	World *world = Auth::get(client)->world;

	Packet packet;
	packet.size = 3 + (world->width * world->height);
	packet.raw = (uint8_t*) malloc(packet.size);

	packet.raw[0] = P_GMAP;
	packet.raw[1] = world->width;
	packet.raw[2] = world->height;

	for (unsigned int i = 3; i < packet.size; i++)
		packet.raw[i] = world->tiles[i - 3]->id;

	client->send(&packet);
	free(packet.raw);

}

World::~World() {

	// todo: MUTEXES!!
	// and kick players

	worlds.rem((uintptr_t) this);

	while (entities.size)
		delete (Entity*) entities[0];

	for (unsigned int i = 0; i < (width * height); i++)
		delete tiles[i];

	free(tiles);

}

Tile* World::getTile(Point *pos) {

	unsigned int x = pos->x + (width / 2);
	unsigned int y = pos->y + (height / 2);

	return tiles[(y * width) + x];

}

void World::setTile(Point *pos, uint8_t id) {

	unsigned int x = pos->x + (width / 2);
	unsigned int y = pos->y + (height / 2);

	unsigned int index = (y * width) + x;

	if (tiles[index]->id == id) return;
	tiles[index]->id = id;

	uint8_t data[4];

	data[0] = P_SBLK;
	*((uint16_t*) (data + 1)) = index;
	data[3] = id;

	Packet packet;
	packet.size = 4;
	packet.raw = data;

	Client::broadcast(&packet);

}

int parseWorld(void *, void *, const char *value, void *result) {

	switch (*value) {

		case 'R': *((int*) result) = Tiledef::ROCK;
		break;

		case 'G': *((int*) result) = Tiledef::GRASS;
		break;

		case 'D': *((int*) result) = Tiledef::DIRT;
		break;

		case 'S': *((int*) result) = Tiledef::SAND;
		break;

		case 'W': *((int*) result) = Tiledef::WATER;
		break;

		case 'I': *((int*) result) = Tiledef::ICE;
		break;

		case 'L': *((int*) result) = Tiledef::LAVA;
		break;

		case 'd': *((int*) result) = Tiledef::DOOR;
		break;

		case 's': *((int*) result) = Tiledef::SIGN;
		break;

		case 'C': *((int*) result) = Tiledef::CACTUS;
		break;

		// to indicate error
		default: return 1;

	}

	return NULL;

}

bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		case P_INTR: {

			Auth *auth = Auth::get(client);
			if (!auth->player) break;

			Tile *tile = auth->world->tiles[*((uint16_t*) packet->data)];
			tile->interact(client);

		} break;

		case P_GMAP: World::sendWorld(client);
		break;

		case P_SBLK: {

			Auth *auth = Auth::get(client);
			if (!auth->player) break;

			World *world = auth->world;

			unsigned int index = *((uint16_t*) packet->data);
			world->tiles[index]->id = packet->data[2];

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
