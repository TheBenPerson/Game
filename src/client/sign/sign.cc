#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "point.hh"

static GFX::texture tex;
static char *text = NULL;

static bool tickNet(Packet *packet);
static void draw();
static void tickInput();

extern "C" {

	bool init() {

		tex = GFX::loadTexture("sign.png");

		Net::listeners.add((uintptr_t) &tickNet);
		GFX::listeners.add((uintptr_t) &draw);
		Input::listeners.add((intptr_t) &tickInput);

		return true;

	}

	void cleanup() {

		Input::listeners.add((intptr_t) &tickInput);
		GFX::listeners.rem((uintptr_t) &draw);
		Net::listeners.rem((uintptr_t) &tickNet);

		GFX::freeTexture(&tex);

	}

}

bool tickNet(Packet *packet) {

	if (packet->id != P_SIGN) return false;

	if (text) free(text);
	text = strdup((char*) packet->data);

	return true;

}

void draw() {

	if (!text) return;

	Point pos = {0, 0};
	Point dim = {16, 16};
	GFX::drawSprite(tex, &pos, &dim);

	pos = {-7, 5};
	GFX::drawText(text, &pos);

}

void tickInput() {

	if (Input::wasCursor) return;

	// todo: mutex!!

	if (!Input::actions[Input::PRIMARY].state && Input::actions[Input::PRIMARY].changed && text) {

		free(text);
		text = NULL;
		return;

	}

}
