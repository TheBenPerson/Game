#ifndef GAME_SERVER_FIREBALL
#define GAME_SERVER_FIREBALL

#include "entity.hh"
#include "packet.hh"

class Fireball: public Entity {

	public:

		Fireball(Point *pos, float rot, Point *vel = NULL);

		bool tick(timespec *time);
		void toNet(Packet *packet);

	private:

		float rot;

};

#endif
