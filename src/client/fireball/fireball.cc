#include <GL/gl.h>
#include <math.h> // for deg to rad
#include <string.h>

#include "console.hh"
#include "fireball.hh"
#include "gfx.hh"
#include "net.hh"

static GLuint tex;

static void initGL();
static void cleanupGL();
static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		GFX::call(&initGL);
		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'fireball.so'");

		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);
		GFX::call(&cleanupGL);

		cputs(YELLOW, "Unloaded module: 'fireball.so'");

	}

}

void initGL() {

	tex = GFX::loadTexture("fireball.png");

}

void cleanupGL() {

	glDeleteTextures(1, &tex);

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GENT: {

			struct Data {

				uint16_t id;

				__attribute__((packed)) Point dim;
				__attribute__((packed)) Point pos;
				__attribute__((packed)) Point vel;

				float rot;
				bool onfire;

				char type[];

			} __attribute__((packed)) *data = (Data*) packet->data;

			if (strcmp(data->type, "fireball")) return false;
			new Fireball((void*) data);

		} break;

		default: return false;

	}

	return true;

}

Fireball::Fireball(void *info): Entity(info) {}

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

	// draw particles
	Entity::draw();

}
