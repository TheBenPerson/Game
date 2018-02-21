#ifndef GAME_CLIENT_ENTITY

#include "nodelist.hh"
#include "point.hh"

class Entity {

	public:

		static NodeList entities;

		unsigned int id;

		Point dim;
		Point pos;
		Point vel;

		float rot;
		bool onfire;

		static Entity* get(unsigned int id);

		Entity(void *data);
		virtual ~Entity();

		virtual void draw();
		virtual void update(void *info);

};

#define GAME_CLIENT_ENTITY
#endif
