#ifndef GAME_CLIENT_ENTITY

#include "nodelist.hh"
#include "point.hh"

class Entity {

	public:

		static NodeList entities;

		unsigned int id;

		Point pos;
		Point vel;

		static Entity* get(unsigned int id);

		Entity(unsigned int id, Point pos, Point vel);
		virtual ~Entity();

		virtual void draw() = 0;
		virtual void update(void *raw);

};

#define GAME_CLIENT_ENTITY
#endif
