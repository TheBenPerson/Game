#ifndef GAME_CLIENT_ENTITY
#define GAME_CLIENT_ENTITY

#include <stdint.h>

#include "nodelist.hh"
#include "point.hh"

class Entity {

	public:

		// base packet for updating entities

		typedef struct {

			uint16_t id;

			__attribute__((packed)) Point dim;
			__attribute__((packed)) Point pos;
			__attribute__((packed)) Point vel;

			char type[10];

		} __attribute__((packed)) UPacket;

		static NodeList entities;

		unsigned int id;

		Point dim;
		Point pos;
		Point vel;

		static Entity* get(unsigned int id);
		static bool verify(UPacket *packet, char *type);

		Entity(void *info);
		virtual ~Entity();

		virtual void draw() = 0;
		virtual void update(uint8_t *info);

};

#endif
