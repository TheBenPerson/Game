#ifndef GAME_CLIENT_AUDIO_SOURCE
#define GAME_CLIENT_AUDIO_SOURCE

#include <AL/al.h>

#include "point.hh"
#include "sound.hh"

class AudioSource {

	public:

		static float rangeMin;
		static float rangeMax;

		AudioSource(Sound *sound);
		~AudioSource();

		void setPos(Point *pos);
		void play(bool loop = false);
		void stop();

	private:

		ALuint source = NULL;

};

#endif
