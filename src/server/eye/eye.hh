#ifndef GAME_SERVER_EYE

#include <time.h>

#include "entity.hh"
#include "point.hh"

class Eye: public Entity {

	public:

		Eye(Point pos, Point vel);

		unsigned int tick(timespec *time);

	private:

		char* getType();

};

#define GAME_SERVER_EYE
#endif
