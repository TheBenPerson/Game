#ifndef GAME_CLIENT_HUMAN
#define GAME_CLIENT_HUMAN

#include "entity.hh"

class Human: public Entity {

	public:

		Human(void *info);

		void draw();

	private:

		typedef enum {LEFT, DOWN, RIGHT, UP} direction;
		direction lastDir = UP;

};

#endif
