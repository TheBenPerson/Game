#ifndef GAME_SERVER_EYE
#define GAME_SERVER_EYE

#include <time.h>

#include "entity.hh"
#include "point.hh"

class Eye: public Entity {

	public:

		Eye(Point *pos, Point *vel = NULL);

		bool tick(timespec *time);

};

#endif
