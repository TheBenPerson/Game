#ifndef GAME_SERVER_FIREBALL
#define GAME_SERVER_FIREBALL

#include "entity.hh"

class Fireball: public Entity {

	public:

		Fireball(Point *pos, float rot, Point *vel = NULL);

		bool tick(timespec *time);
		void* toNet(unsigned int *size);

	private:

		float rot;

};

#endif
