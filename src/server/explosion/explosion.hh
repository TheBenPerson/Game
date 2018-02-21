#ifndef GAME_SERVER_EXPLOSION
#define GAME_SERVER_EXPLOSION

#include <time.h>

#include "entity.hh"

class Explosion: public Entity {

	public:

		Explosion(Point *pos, float radius = 2, float duration = .5f);

		bool tick(timespec *time);

	private:

		unsigned int duration;

};

#endif
