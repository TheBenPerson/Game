#include <stdint.h>

#include "audio_source.hh"
#include "explosion.hh"
#include "gfx.hh"
#include "sound.hh"
#include "world.hh"

static GFX::texture tex;
static Sound *sound;

static void create(uint8_t *data);

extern "C" {

	bool init() {

		Entity::regEnt("explosion", &create);
		tex = GFX::loadTexture("explosion.png");
		sound = new Sound("explosion.ogg");

		return true;

	}

	void cleanup() {

		delete sound;

		// todo: unreg ent?
		GFX::freeTexture(&tex);

	}

}

Explosion::Explosion(uint8_t *data): Entity(data) {

	source = new AudioSource(sound);
	source->setPos(&pos);
	source->play();

}

Explosion::~Explosion() {

	delete source;

}

void Explosion::draw() {

	Entity::draw();

	Point tdim = {7, 1};
	Point frame = {(float) (GFX::frame / 5), 0};
	GFX::drawSprite(tex, &pos, &dim, 0, &tdim, &frame);

}

void create(uint8_t *data) {

	new Explosion(data);

}
