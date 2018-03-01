#include <GL/gl.h>
#include <math.h> // for deg to rad

#include "console.hh"
#include "fireball.hh"
#include "gfx.hh"
#include "net.hh"

static GFX::texture tex;

static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("fireball.png");
		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'fireball.so'");

		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);
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

	Point dpos = vel * (1 / 60.0f);
	pos += dpos;

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0);
	glRotatef(((rot + M_PI_2) * 360) / (M_PI * 2), 0, 0, 1);
	glScalef(dim.x / 2, dim.y / 2, 1);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef((GFX::frame / 5) / 8.0f, 0, 0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1 / 8.0f, 1);
	glVertex2f(1, 1);
	glTexCoord2f(1 / 8.0f, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);

	glEnd();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
