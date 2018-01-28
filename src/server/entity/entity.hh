#ifndef GAME_SERVER_ENTITY

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

		virtual void tick() = 0;
		virtual void* toNet(unsigned int *size);

	private:

		virtual char* getType() = 0;

};

#define GAME_SERVER_ENTITY
#endif
