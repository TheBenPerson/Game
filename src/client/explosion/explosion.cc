#include <GL/gl.h>
#include <string.h>

#include "console.hh"
#include "explosion.hh"
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

		cputs(GREEN, "Loaded module: 'explosion.so'");

		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);
		GFX::call(&cleanupGL);

		cputs(YELLOW, "Unloaded module: 'explosion.so'");

	}

}

void initGL() {

	tex = GFX::loadTexture("explosion.png");

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

			if (strcmp(data->type, "explosion")) return false;
			new Explosion((void*) data);

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
