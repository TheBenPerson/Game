#include <AL/al.h>

#include "audio_source.hh"
#include "sound.hh"

float AudioSource::rangeMin;
float AudioSource::rangeMax;

AudioSource::AudioSource(Sound *sound) {

	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, sound->sound);

	alSourcef(source, AL_REFERENCE_DISTANCE, rangeMin);
	alSourcef(source, AL_MAX_DISTANCE, rangeMax);

}

AudioSource::~AudioSource() {

	alDeleteSources(1, &source);

}

void AudioSource::setPos(Point *pos) {

	alSource3f(source, AL_POSITION, pos->x, 0, pos->y);

}

void AudioSource::play(bool loop) {

	alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	alSourcePlay(source);

}

void AudioSource::stop() {

	alSourceStop(source);

}
