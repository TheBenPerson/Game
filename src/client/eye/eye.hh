#ifndef GAME_CLIENT_EYE

#include "entity.hh"

class Eye: public Entity {

	public:

		Eye(void *data);

		void draw();

};

#define GAME_CLIENT_EYE
#endif
