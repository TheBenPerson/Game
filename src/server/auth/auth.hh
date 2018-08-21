#ifndef GAME_SERVER_AUTH
#define GAME_SERVER_AUTH

#include "client.hh"
#include "nodelist.hh"
#include "player.hh"
#include "world.hh"

class Auth {

	public:

		Client *client;
		Player *player = NULL;
		World *world = World::defaultWorld;

		static Auth* get(Client *client);

		Auth(Client *client);
		~Auth();

};

#endif
