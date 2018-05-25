#include <math.h>
#include <stdint.h>

#include "client.hh"
#include "console.hh"
#include "entity.hh"
#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "packet.hh"
#include "win.hh"
#include "world.hh"

static Point lastVel = {0, 0};
static float speed = 2;

static Entity *target = NULL;

static void draw();
static void handlerInput();
static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		GFX::listeners.add((intptr_t) &draw);

		Net::listeners.add((intptr_t) &tickNet);
		Net::send(P_NEWP);

		Input::listeners.add((intptr_t) &handlerInput);

		cputs(GREEN, "Loaded module: 'player.so'");

		return true;

	}

	void cleanup() {

		World::pos = NULL;

		Input::listeners.rem((intptr_t) &handlerInput);
		Net::listeners.rem((intptr_t) &tickNet);

		GFX::listeners.rem((intptr_t) &draw);

		cputs(YELLOW, "Unloaded module: 'player.so'");

	}

}

void draw() {

	/* TODO: put this back in world.cc

	// TODO: move me
	if (Input::actions[Input::EXIT]) Client::state = Client::PAUSED;*/

	if (!Input::actions[Input::MODIFIER].state)
		World::rot = (Input::cursor.x / (10 * WIN::aspect)) * 2 * M_PI;

	Point vel = {0, 0};

	if (Input::actions[Input::UP].state) {

		vel.x += sinf(World::rot);
		vel.y += cosf(World::rot);

	}

	if (Input::actions[Input::DOWN].state) {

		vel.x -= sinf(World::rot);
		vel.y -= cosf(World::rot);

	}

	if (Input::actions[Input::LEFT].state) {

		vel.x += sinf(World::rot - M_PI_2);
		vel.y += cosf(World::rot - M_PI_2);

	}

	if (Input::actions[Input::RIGHT].state) {

		vel.x -= sinf(World::rot - M_PI_2);
		vel.y -= cosf(World::rot - M_PI_2);

	}

	vel.normalize();
	vel *= speed;

	if (vel == lastVel) return;
	lastVel = vel;

	target->vel = vel;

	struct {

		uint8_t id = P_UPDP;
		__attribute__((packed)) Point pos;
		__attribute__((packed)) Point vel;

	} __attribute__((packed)) data;

	data.pos = *World::pos;
	data.vel = vel;

	Packet packet;
	packet.raw = (uint8_t*) &data;
	packet.size = sizeof(data);

	Net::send(&packet);

}

void handlerInput() {

	if (Input::actions[Input::MODIFIER].changed)
		WIN::setCursor(Input::actions[Input::MODIFIER].state);

	if (Input::actions[Input::MODIFIER].state) {
	if (Input::actions[Input::PRIMARY].changed && Input::actions[Input::PRIMARY].state) {

		Point pos = Input::cursor;
		pos /= World::scale;
		pos.rot(-World::rot);
		pos += *World::pos;

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

bool tickNet(Packet *packet) {

	if (packet->id != P_IDNT) return false;

	unsigned int id = *((uint16_t*) packet->data);
	target = Entity::get(id);

	// todo: seems a little redundant
	if (target) World::pos = &target->pos;
	else Net::send(P_NEWP);

	return true;

}
