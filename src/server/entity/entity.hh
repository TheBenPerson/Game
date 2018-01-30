#ifndef GAME_SERVER_ENTITY

#include <time.h>

#include "nodelist.hh"
#include "point.hh"

class Entity {

	public:

		static NodeList entities;

		unsigned int id;

		Point pos;
		Point vel;

		static Entity* get(unsigned int id);

		Entity(Point pos, Point vel);
		virtual ~Entity();

		virtual unsigned int tick(timespec *time) = 0;
		virtual void* toNet(unsigned int *size);
		virtual void update(); // todo: remove virtual?

	protected:

		unsigned int lastNano = NULL;
		unsigned int lastSec = NULL;

	private:

		virtual char* getType() = 0;

};

#define GAME_SERVER_ENTITY
#endif
