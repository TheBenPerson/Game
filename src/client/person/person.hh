#ifndef GAME_CLIENT_PERSON
#define GAME_CLIENT_PERSON

#include "entity.hh"

class Person: public Entity {

	public:

		Person(void *info);

		void draw();

};

#endif
