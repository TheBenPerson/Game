#ifndef GAME_SERVER_DOOR
#define GAME_SERVER_DOOR

#include "client.hh"
#include "tile.hh"

class Door: public Tile {

	public:

		Door(char *text = NULL, bool freeStr = false);

		virtual void interact(Client *client);

};

#endif
