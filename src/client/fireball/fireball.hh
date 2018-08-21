#ifndef GAME_CLIENT_FIREBALL
#define GAME_CLIENT_FIREBALL

#include <stdint.h>

#include "entity.hh"

class Fireball: public Entity {

	public:

		Fireball(uint8_t *data);

		void draw();

	private:

		float rot;

};

#endif
