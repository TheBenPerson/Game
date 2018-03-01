#ifndef GAME_SERVER_ENTITY
#define GAME_SERVER_ENTITY

#include <stdint.h>
#include <time.h>

#include "nodelist.hh"
#include "point.hh"

class Entity {

	public:

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
		Point pos = {0, 0};
		Point vel = {0, 0};

		bool onfire = false;

		volatile long time = NULL;
		volatile unsigned int timer = 0;
		unsigned int interval = 50;

		const char *type = "base";

		static Entity* get(unsigned int id);

		virtual ~Entity();

		void add();
		bool bound(Point *pos, Point *dim);
		unsigned int boundWorld(Point **tiles);

		virtual bool tick(timespec *time) = 0;
		virtual void* toNet(unsigned int *size);
		virtual void update(); // todo: remove virtual?

};

#endif
