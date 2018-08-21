#ifndef GAME_SERVER_SIGN
#define GAME_SERVER_SIGN

#include "client.hh"
#include "tile.hh"

class Sign: public Tile {

	public:

		static void send(Client *client, char *str);
		static void send(char *str);

		Sign(char *text = NULL, bool freeStr = false);

		virtual void interact(Client *client);

};

#endif
