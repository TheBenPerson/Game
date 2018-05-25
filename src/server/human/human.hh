#ifndef GAME_SERVER_HUMAN
#define GAME_SERVER_HUMAN

#include "client.hh"
#include "entity.hh"
#include "nodelist.hh"

class Human: public Entity {

	public:

		Human(Client *client);

		bool tick(timespec *time);

};

#endif
