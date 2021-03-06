#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "client.hh"
#include "nodelist.hh"
#include "tile.hh"

typedef struct {

	uint8_t id;
	Tile* (*create)(void*, bool);

} Assoc;

static NodeList assocs;

extern "C" {

	bool init() {

		return true;

	}

	void cleanup() {

		for (unsigned int i = 0; i < assocs.size; i++)
			free((void*) assocs[i]);

	}

}

void Tile::regTile(uint8_t id, Tile* (*create)(void*, bool)) {

	Assoc *assoc = (Assoc*) malloc(sizeof(Assoc));
	assoc->id = id;
	assoc->create = create;

	assocs.add((uintptr_t) assoc);

}

Tile* Tile::newTile(uint8_t id, void *special, bool freeSpecial) {

	for (unsigned int i = 0; i < assocs.size; i++) {

		Assoc *assoc = (Assoc*) assocs[i];
		if (assoc->id == id) return assoc->create(special, freeSpecial);

	}

	return new Tile(id, special, freeSpecial);

}

Tile::Tile(uint8_t id, void *special, bool freeSpecial): id(id), special(special), freeSpecial(freeSpecial) {}

Tile::~Tile() {

	if (freeSpecial) free(special);

}

void Tile::interact(Client *client) {}
