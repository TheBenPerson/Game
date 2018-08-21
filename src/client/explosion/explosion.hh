#ifndef GAME_CLIENT_EXPLOSION
#define GAME_CLIENT_EXPLOSION

#include <stdint.h>

#include "audio_source.hh"
#include "entity.hh"

class Explosion: public Entity {

	public:

		Explosion(uint8_t *data);
		~Explosion();

		void draw();

	private:

		AudioSource *source;

};

#endif
