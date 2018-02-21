#ifndef GAME_CLIENT_EXPLOSION
#define GAME_CLIENT_EXPLOSION

#include "entity.hh"

class Explosion: public Entity {

	public:

		Explosion(void *data);

		void draw();

};

#endif
