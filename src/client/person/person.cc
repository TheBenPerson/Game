#include <GL/gl.h>
#include <math.h>

#include "console.hh"
#include "gfx.hh"
#include "net.hh"
#include "person.hh"
#include "world.hh"

GFX::texture tex;

static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("healer_m.png");
		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'person.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);
		GFX::freeTexture(&tex);

		cputs(YELLOW, "Unloaded module: 'person.so'");

	}

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GENT: {

			Entity::UPacket *upacket = (Entity::UPacket *) packet->data;

			if (!Entity::verify(upacket, "person")) return false;
			new Person((void*) upacket);

		} break;

		default: return false;

	}

	return true;

}

Person::Person(void *info): Entity(info) {}

void Person::draw() {

	Point dpos = vel * (1 / 60.0f);
	pos += dpos;

	Point pvel = vel;
	pvel.rot(World::rot);

	enum {LEFT, DOWN, RIGHT, UP} dir;

	if (fabsf(pvel.x) > fabsf(pvel.y)) {

		if (pvel.x < 0) dir = LEFT;
		else dir = RIGHT;

	} else {

		if (pvel.y < 0) dir = DOWN;
		else dir = UP;

	}

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0);
	glRotatef(-(World::rot * 360) / (M_PI * 2), 0, 0, 1);
	glScalef(dim.x / 2, dim.y / 2, 1);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glScalef(1 / 3.0f, .25f, 1);
	glTranslatef(GFX::frame / 20, dir, 0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);

	glEnd();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
