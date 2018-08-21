#ifndef GAME_CLIENT_EYE
#define GAME_CLIENT_EYE

#include <stdint.h>

#include "entity.hh"

class Eye: public Entity {

	public:

		Eye(uint8_t *data);

		void draw();

};

#endif
