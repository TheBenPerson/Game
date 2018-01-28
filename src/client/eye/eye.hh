#ifndef GAME_CLIENT_EYE

#include "entity.hh"
#include "point.hh"

class Eye: public Entity {

	public:

		Eye(unsigned int id, Point pos, Point vel);

		void draw();

};

#define GAME_CLIENT_EYE
#endif
