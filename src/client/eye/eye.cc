#include <GL/gl.h>
#include <math.h>
#include <stdint.h>

#include "client.hh"
#include "console.hh"
#include "eye.hh"
#include "gfx.hh"
#include "net.hh"
#include "world.hh"

static GFX::texture tex;

static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("will.png");
		Net::listeners.add((intptr_t) &tickNet);

		cputs(GREEN, "Loaded module: 'eye.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((intptr_t) &tickNet);
		GFX::freeTexture(&tex);

		cputs(YELLOW, "Unloaded module: 'eye.so'");

	}

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GENT: {

			Entity::UPacket *upacket = (Entity::UPacket*) packet->data;

			if (!Entity::verify(upacket, "eye")) return false;
			new Eye((void*) upacket);

		} break;

		default: return false;

	}

	return true;

}

Eye::Eye(void *info): Entity(info) {}

void Eye::draw() {

	Point dpos = vel * (1 / 60.0f);
	pos += dpos;

	Point tdim = {2, 1};
	Point frame = {(float) (GFX::frame / 20), 0};
	GFX::drawSprite(tex, &pos, &dim, GFX::frame / 20.0f, &tdim, &frame);

}
