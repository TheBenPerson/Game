#ifndef GAME_CLIENT_ENTITY
#define GAME_CLIENT_ENTITY

#include <stdint.h>

#include "nodelist.hh"
#include "point.hh"

// todo: const int inside Entity class?
#define SIZE_TENTITY (2 + 10 + (SIZE_TPOINT * 3))

class Entity {

	public:

		static NodeList entities;

		unsigned int id;

		Point dim;
		Point pos;
		Point vel;

		static Entity* get(unsigned int id);
		static void regEnt(const char *name, void (*create)(uint8_t*));

		Entity(uint8_t *buff);
		virtual ~Entity();

		void unpack(uint8_t *buff);
		virtual void draw() = 0;

};

#endif
