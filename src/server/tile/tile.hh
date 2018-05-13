#ifndef GAME_SERVER_TILE
#define GAME_SERVER_TILE

#include <stdint.h>

#include "client.hh"

class Tile {

	public:

		uint8_t id;
		void *special;
		bool freeSpecial;

		static void regTile(uint8_t id, Tile* (*create)(void*, bool));
		static Tile* newTile(uint8_t id, void *special = NULL, bool freeSpecial = false);

		Tile(uint8_t id, void *special = NULL, bool freeSpecial = false);
		virtual ~Tile();

		virtual void interact(Client *client);

};

#endif
