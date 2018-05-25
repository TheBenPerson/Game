#ifndef GAME_SERVER_ENTITY
#define GAME_SERVER_ENTITY

#include <stdint.h>
#include <time.h>

#include "client.hh"
#include "nodelist.hh"
#include "packet.hh"
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
		Client *client = NULL;

		Point dim = {1, 1};
		Point pos = {0, 0};
		Point vel = {0, 0};

		bool onfire = false;

		volatile long time = NULL;
		volatile unsigned int timer = 0;
		unsigned int interval = 50;

		const char *type = "base";

		static Entity* get(unsigned int id);
		static Entity* get(Client *client);

		Entity();
		Entity(Client *client);
		virtual ~Entity();

		bool bound(Point *pos, Point *dim);
		unsigned int boundWorld(Point **tiles);

		virtual bool tick(timespec *time) = 0;
		virtual void toNet(Packet *packet);
		void send();
		virtual void update();

};

#endif
