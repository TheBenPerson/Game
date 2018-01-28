#ifndef GAME_SERVER_EYE

#include "entity.hh"
#include "point.hh"

class Eye: public Entity {

	public:

		Eye(Point pos, Point vel);

		void tick();

	private:

		char* getType();

};

#define GAME_SERVER_EYE
#endif
