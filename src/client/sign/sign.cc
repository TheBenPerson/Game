#include <GL/gl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "console.hh"
#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "point.hh"

static GFX::texture tex;
static char *text = NULL;

static bool tickNet(Packet *packet);
static void draw();

extern "C" {

	bool init() {

		tex = GFX::loadTexture("sign.png");

		Net::listeners.add((intptr_t) &tickNet);
		GFX::listeners.add((intptr_t) &draw);

		cputs(GREEN, "Loaded module: 'sign.so'");
		return true;

	}

	void cleanup() {

		GFX::listeners.rem((intptr_t) &draw);
		Net::listeners.rem((intptr_t) &tickNet);

		GFX::freeTexture(&tex);

		cputs(YELLOW, "Unloaded module: 'sign.so'");

	}

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

	if (Input::actions[Input::PRIMARY].state) {

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

	Point pos = {-7, 5};
	GFX::drawText(text, &pos);

}
