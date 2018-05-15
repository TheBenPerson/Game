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

		tex = GFX::loadTexture("eye.png");
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

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0);
	glScalef(dim.x / 2, dim.y / 2, 1);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef((GFX::frame / 2) / 16.0f, 0, 0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1 / 16.0f, 1);
	glVertex2f(1, 1);
	glTexCoord2f(1 / 16.0f, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);

	glEnd();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
