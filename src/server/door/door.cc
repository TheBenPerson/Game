#include <stdint.h>

#include "auth.hh"
#include "door.hh"
#include "tile.hh"
#include "tiledef.hh"
#include "world.hh"

static Tile* newDoor(void *text, bool freeSpecial);

extern "C" {

	bool init() {

		Tile::regTile(Tiledef::DOOR, &newDoor);
		return true;

	}

	void cleanup() {

		// todo: unregister?

	}

}

Door::Door(char *text, bool freeStr): Tile(Tiledef::DOOR, text, freeStr) {}

void Door::interact(Client *client) {

	World *world = World::get((char*) special);
	if (!world) world = World::newWorld((char*) special);
	if (!world) return;

	// todo: put auth up one function
	// like in world or something

	Auth *auth = Auth::get(client);
	auth->player->pos = {0, 0};
	auth->player->vel = {0, 0};

	auth->world = world;
	auth->player->transfer(world);

	World::sendWorld(client);

}

Tile* newDoor(void *text, bool freeSpecial) {

	return new Door((char*) text, freeSpecial);

}
