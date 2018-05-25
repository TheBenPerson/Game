#include <GL/gl.h>
#include <stdint.h>

#include "console.hh"
#include "explosion.hh"
#include "gfx.hh"
#include "net.hh"

static GFX::texture tex;

static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("explosion.png");
		Net::listeners.add((intptr_t) &tickNet);

		cputs(GREEN, "Loaded module: 'explosion.so'");

		return true;

	}

	void cleanup() {

		Net::listeners.rem((intptr_t) &tickNet);
		GFX::freeTexture(&tex);

		cputs(YELLOW, "Unloaded module: 'explosion.so'");

	}

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GENT: {

			Entity::UPacket *upacket = (Entity::UPacket*) packet->data;

			if (!Entity::verify(upacket, "explosion")) return false;
			new Explosion((void*) upacket);

		} break;

		default: return false;

	}

	return true;

}

Explosion::Explosion(void *info): Entity(info) {}

void Explosion::draw() {

	Entity::draw();

	Point tdim = {7, 1};
	Point frame = {(float) (GFX::frame / 5), 0};
	GFX::drawSprite(tex, &pos, &dim, 0, &tdim, &frame);

}
