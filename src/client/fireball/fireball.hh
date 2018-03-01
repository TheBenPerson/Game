#ifndef GAME_CLIENT_FIREBALL

#include "entity.hh"

class Fireball: public Entity {

	public:

		Fireball(void *data);

		void draw();

	private:

		float rot;

};

#define GAME_CLIENT_FIREBALL
#endif
