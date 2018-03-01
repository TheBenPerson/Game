#include <GL/gl.h>

#include "console.hh"
#include "explosion.hh"
#include "gfx.hh"
#include "net.hh"

static GFX::texture tex;

static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("explosion.png");
		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'explosion.so'");

		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);
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

	Point dpos = vel * (1 / 60.0f);
	pos += dpos;

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0);
	glScalef(dim.x / 2, dim.y / 2, 1);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef((GFX::frame / 5) / 7.0f, 0, 0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1 / 7.0f, 1);
	glVertex2f(1, 1);
	glTexCoord2f(1 / 7.0f, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);

	glEnd();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
