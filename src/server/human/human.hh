#ifndef GAME_SERVER_HUMAN
#define GAME_SERVER_HUMAN

#include "client.hh"
#include "entity.hh"
#include "packet.hh"
#include "world.hh"

class Human: public Entity {

	public:

		Human(World *world, bool send = true);

		virtual bool tick(unsigned int time);
		virtual void toNet(Packet *packet);

	protected:

		char *name = NULL;

};

#endif
