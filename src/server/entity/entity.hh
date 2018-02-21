#ifndef GAME_SERVER_ENTITY
#define GAME_SERVER_ENTITY

#include <time.h>

#include "nodelist.hh"
#include "point.hh"

class Entity {

	public:

		static NodeList entities;

		unsigned int id;

		Point dim;
		Point pos = {0, 0};
		Point vel = {0, 0};
		float rot;

		bool onfire = false;

		volatile long time = NULL;
		volatile unsigned int timer = 0;
		unsigned int interval = 50;

		const char *type = "base";

		static Entity* get(unsigned int id);

		virtual ~Entity();

		void add();
		bool bound(Point *pos, Point *dim, float rot);
		unsigned int boundWorld(Point **tiles);

		virtual bool tick(timespec *time) = 0;
		virtual void* toNet(unsigned int *size);
		virtual void update(); // todo: remove virtual?

};

#endif
