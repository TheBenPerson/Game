#ifndef GAME_SERVER_EYE
#define GAME_SERVER_EYE

#include "entity.hh"
#include "point.hh"

class Eye: public Entity {

	public:

		Eye(World *world, Point *pos, Point *vel = NULL);

		bool tick(unsigned int time);

};

#endif
