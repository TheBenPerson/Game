#include <math.h>
#include <stdint.h>

#include "client.hh"
#include "entity.hh"
#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "packet.hh"
#include "sound.hh"
#include "timing.hh"
#include "win.hh"
#include "world.hh"

static GFX::texture tex;
static Point lastVel = {0, 0};
static float speed = 2.3f;

static Timing::Condition condition;
static Entity *target = NULL;

static void draw();
static void handlerInput();
static bool tickNet(Packet *packet);

extern "C" {

	bool init() {

		tex = GFX::loadTexture("health.png");
		GFX::listeners.add((uintptr_t) &draw);

		Net::listeners.add((uintptr_t) &tickNet);

		// await immediate response

		for (;;) {

			Net::send(P_IDNT);
			bool result = Timing::waitFor(&condition, 1000);
			if (result) break;

		}

		Input::listeners.add((uintptr_t) &handlerInput);

		return true;

	}

	void cleanup() {

		World::pos = NULL;

		Input::listeners.rem((uintptr_t) &handlerInput);
		Net::listeners.rem((uintptr_t) &tickNet);

		GFX::listeners.rem((uintptr_t) &draw);
		GFX::freeTexture(&tex);

	}

}

void draw() {

	Point dim = {54, 17};
	dim /= dim.y;

	Point pos = {WIN::aspect * -10, -10};
	Point ddim = dim / 2.0f;
	pos += ddim;

	Point tdim = {1, 7};
	Point frame = {1, (float) (GFX::frame / 7)};

	GFX::drawSprite(tex, &pos, &dim, NULL, &tdim, &frame);

	/* TODO: put this back in world.cc
	if (Input::actions[Input::EXIT]) Client::state = Client::PAUSED;*/

	if (!Input::actions[Input::MODIFIER].state) {

		World::rot = (Input::cursor.x / (10 * WIN::aspect)) * 2 * M_PI;
		Sound::setRot(-World::rot);

	}

	// todo: consider moving?
	Sound::setPos(&target->pos);

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

	// set player entity vel
	target->vel = vel;

	// tell server

	uint8_t data[1 + (SIZE_TPOINT * 2)];

	data[0] = P_UPDP;
	World::pos->pack(data + 1);
	vel.pack(data + 1 + SIZE_TPOINT);

	Packet packet;
	packet.raw = data;
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

		uint8_t data[3];

		data[0] = P_INTR;
		*((uint16_t*) (data + 1)) = World::getIndex(&pos);

		Packet packet;
		packet.raw = data;
		packet.size = 3;

		Net::send(&packet);

	}}

}

bool tickNet(Packet *packet) {

	if (packet->id != P_IDNT) return false;

	unsigned int id = *((uint16_t*) packet->data);
	target = Entity::get(id);

	if (target) {

		World::pos = &target->pos;
		Timing::signal(&condition);

	}

	return true;

}
