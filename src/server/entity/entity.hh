#ifndef GAME_SERVER_ENTITY
#define GAME_SERVER_ENTITY

#include <stdint.h>

#include "client.hh"
#include "nodelist.hh"
#include "packet.hh"
#include "point.hh"
#include "world.hh"

#define SIZE_TENTITY (2 + 10 + (SIZE_TPOINT * 3))

class Entity {

	public:

		unsigned int id;
		World *world;

		Point dim = {1, 1};
		Point pos = {0, 0};
		Point vel = {0, 0};

		bool onfire = false;

		volatile long time = NULL;
		volatile unsigned int timer = 0;
		unsigned int interval = 50;

		const char *type = "base";

		static Entity* get(unsigned int id);

		Entity(World *world);
		virtual ~Entity();

		bool bound(Point *pos, Point *dim);
		unsigned int boundWorld(Point **tiles);
		void pack(uint8_t *buff);
		void send();

		void transfer(World *world);

		virtual bool tick(unsigned int time) = 0;
		virtual void toNet(Packet *packet);
		virtual void update();

};

#endif
