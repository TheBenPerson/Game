#ifndef GAME_CLIENT_HUMAN
#define GAME_CLIENT_HUMAN

#include <stdint.h>

#include "entity.hh"

class Human: public Entity {

	public:

		Human(uint8_t *datat);
		~Human();

		void draw();

	private:

		typedef enum {LEFT, DOWN, RIGHT, UP} direction;
		direction lastDir = UP;

		char *name;

};

#endif
