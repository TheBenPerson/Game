#include <math.h>
#include <stdint.h>

#include "client.hh"
#include "console.hh"
#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "packet.hh"
#include "win.hh"
#include "world.hh"

static Point lastVel = {0, 0};
static float speed = 2;
static GFX::texture tex;

static void draw();
static void handlerInput();

extern "C" {

	bool init() {

		Net::send(P_NEWP);

		tex = GFX::loadTexture("player.png");
		GFX::listeners.add((void*) &draw);

		Input::listeners.add((void*) &handlerInput);

		cputs(GREEN, "Loaded module: 'player.so'");

		return true;

	}

	void cleanup() {

		Input::listeners.rem((void*) &handlerInput);

		GFX::listeners.rem((void*) &draw);
		GFX::freeTexture(&tex);

		cputs(YELLOW, "Unloaded module: 'player.so'");

	}

}

void draw() {

	/* TODO: put this back in world.cc

	if (Input::actions[Input::A_SECONDARY]) {

		if (Input::actions[Input::A_UP]) scale += speed;
		else if (Input::actions[Input::A_DOWN]) scale -= speed;

		return;

	}
	*/

	// TODO: move me
	// if (Input::actions[Input::EXIT]) Client::state = Client::PAUSED;

	Point vel = {0, 0};

	if (Input::actions[Input::UP].state) {

		vel.x += sinf(World::rot) * speed;
		vel.y += cosf(World::rot) * speed;

	}

	if (Input::actions[Input::DOWN].state) {

		vel.x -= sinf(World::rot) * speed;
		vel.y -= cosf(World::rot) * speed;

	}

	if (Input::actions[Input::LEFT].state) {

		vel.x += sinf(World::rot - M_PI_2) * speed;
		vel.y += cosf(World::rot - M_PI_2) * speed;

	}

	if (Input::actions[Input::RIGHT].state) {

		vel.x -= sinf(World::rot - M_PI_2) * speed;
		vel.y -= cosf(World::rot - M_PI_2) * speed;

	}

	Point dpos = vel * (1 / 60.0f);
	World::pos += dpos;

	if (vel == lastVel) return;
	lastVel = vel;

	struct {

		uint8_t id = P_UPDP;
		__attribute__((packed)) Point pos;
		__attribute__((packed)) Point vel;

	} __attribute__((packed)) data;

	data.pos = World::pos;
	data.vel = vel;

	Packet packet;
	packet.raw = (uint8_t*) &data;
	packet.size = sizeof(data);

	Net::send(&packet);

}

void handlerInput() {

	if (!Input::actions[Input::MODIFIER].state && Input::wasCursor) {

		World::rot = (Input::cursor.x / (10 * WIN::aspect)) * 2 * M_PI;
		return;

	}

	if (Input::actions[Input::MODIFIER].changed) {

		WIN::setCursor(Input::actions[Input::MODIFIER].state);

	}

	if (Input::actions[Input::MODIFIER].state) {
	if (Input::actions[Input::PRIMARY].changed && Input::actions[Input::PRIMARY].state) {

		Point pos = Input::cursor;
		pos /= World::scale;
		pos.rot(-World::rot);
		pos += World::pos;

		unsigned int index = World::getIndex(&pos);

		struct {

			uint8_t id = P_INTR;
			uint16_t index;

		} __attribute__((packed)) data;

		data.index = index;

		Packet packet;
		packet.raw = (uint8_t*) &data;
		packet.size = sizeof(data);

		Net::send(&packet);

	}}

}
