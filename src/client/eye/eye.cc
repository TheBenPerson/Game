#include <GL/gl.h>
#include <math.h>
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "eye.hh"
#include "gfx.hh"
#include "net.hh"
#include "world.hh"

static GLuint tex;

static void initGL();
static void cleanupGL();
static bool tickNet(Packet *packet);

extern "C" {

	char* eye_deps[] = {

		"gfx.so",
		"world.so",
		NULL

	};

	bool init() {

		GFX::call(&initGL);
		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'eye.so'");

		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);
		GFX::call(&cleanupGL);

		cputs(YELLOW, "Unloaded module: 'eye.so'");

	}

}

void initGL() {

	tex = GFX::loadTexture("eye.png");

}

void cleanupGL() {

	glDeleteTextures(1, &tex);

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_NENT: {

			typedef struct {

				uint8_t pid;
				uint16_t id;

				__attribute__((packed)) Point pos;
				__attribute__((packed)) Point vel;

				char type[];

			} __attribute__((packed)) Data;
			Data *data = (Data*) packet->raw;

			if (strcmp(data->type, "eye")) return false;
			new Eye(data->id, data->pos, data->vel);

		} break;

		default: return false;

	}

	return true;

}

Eye::Eye(unsigned int id, Point pos, Point vel): Entity(id, pos, vel) {}

void Eye::draw() {

	if (Client::state != Client::IN_GAME) return;

	glPushMatrix();
	glTranslatef(pos.x - .5f, pos.y - .5f, 0);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef((GFX::frame / 2) / 16.0f, 0, 0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(0, 1);
	glTexCoord2f(1 / 16.0f, 1);
	glVertex2f(1, 1);
	glTexCoord2f(1 / 16.0f, 0);
	glVertex2f(1, 0);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);

	glEnd();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	pos += vel;

}
