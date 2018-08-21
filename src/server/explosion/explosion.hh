#ifndef GAME_SERVER_EXPLOSION
#define GAME_SERVER_EXPLOSION

#include "entity.hh"
#include "world.hh"

class Explosion: public Entity {

	public:

		Explosion(World *world, Point *pos, float radius = 2, float duration = .5f);

		bool tick(unsigned int time);

	private:

		unsigned int duration;

};

#endif
