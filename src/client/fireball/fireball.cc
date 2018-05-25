#include <math.h>
#include <stdint.h>

#include "console.hh"
#include "fireball.hh"
#include "gfx.hh"
#include "net.hh"

static GFX::texture tex;

static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("fireball.png");
		Net::listeners.add((intptr_t) &tickNet);

		cputs(GREEN, "Loaded module: 'fireball.so'");

		return true;

	}

	void cleanup() {

		Net::listeners.rem((intptr_t) &tickNet);
		GFX::freeTexture(&tex);

		cputs(YELLOW, "Unloaded module: 'fireball.so'");

	}

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GENT: {

			Entity::UPacket *upacket = (Entity::UPacket*) packet->data;

			if (!Entity::verify(upacket, "fireball")) return false;
			new Fireball((void*) upacket);

		} break;

		default: return false;

	}

	return true;

}

Fireball::Fireball(void *info): Entity(info) {

	struct Data {

		Entity::UPacket packet;
		float rot;

	} __attribute__((packed)) *data = (Data*) info;

	rot = data->rot;

}

void Fireball::draw() {

	Entity::draw();

	Point tdim = {8, 1};
	Point frame = {(float) (GFX::frame / 5), 0};
	GFX::drawSprite(tex, &pos, &dim, rot + M_PI_2, &tdim, &frame);

}
