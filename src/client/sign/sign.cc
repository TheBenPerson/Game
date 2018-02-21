#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>

#include "console.hh"
#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "point.hh"

static GLuint tex;
static char *text = NULL;

static void initGL();
static void cleanupGL();
static bool tickNet(Packet *packet);
static void draw();

extern "C" {

	bool init() {

		GFX::call(&initGL);

		Net::listeners.add((void*) &tickNet);
		GFX::listeners.add((void*) &draw);

		cputs(GREEN, "Loaded module: 'sign.so'");
		return true;

	}

	void cleanup() {

		GFX::listeners.rem((void*) &draw);
		Net::listeners.rem((void*) &tickNet);

		GFX::call(&cleanupGL);

		cputs(YELLOW, "Unloaded module: 'sign.so'");

	}

}

void initGL() {

	tex = GFX::loadTexture("sign.png");

}

void cleanupGL() {

	glDeleteTextures(1, &tex);

}

bool tickNet(Packet *packet) {

	if (packet->id != P_SIGN) return false;

	char *old = text;
	text = strdup((char*) packet->data);
	if (old) free(old);

	return true;

}

void draw() {

	if (!text) return;

	if (Input::actions[Input::A_PRIMARY]) {

		free(text);
		text = NULL;
		return;

	}

	glPushMatrix();
	glScalef(8, 8, 1);

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

	Point pos = {-7, 5.5};
	GFX::drawText(text, &pos);

}
