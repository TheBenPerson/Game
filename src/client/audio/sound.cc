/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2018 Ben Stockett <thebenstockett@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <AL/al.h>
#include <AL/alc.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vorbis/vorbisfile.h>

#include "audio_source.hh"
#include "config.hh"
#include "point.hh"
#include "sound.hh"
#include "string.hh"
#include "world.hh"

static bool enabled;

static unsigned int pageSize;
static ALCdevice *device;
static ALCcontext *context;
static Config *config;

extern "C" {

	bool init() {

		pageSize = sysconf(_SC_PAGESIZE);
		// todo: error handling?

		device = alcOpenDevice(NULL);
		if (!device) return false;

		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);

		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

		Config::Option options[] = {

			BOOL("enabled", true),
			FLOAT("range-min", 1),
			FLOAT("range-max", 15),
			END

		};

		config = new Config("cfg/client/audio.cfg", options);

		enabled = config->getBool("enabled");
		AudioSource::rangeMin = config->getFloat("range-min");
		AudioSource::rangeMax = config->getFloat("range-max");

		return true;

	}

	void cleanup() {

		delete config;

		alcMakeContextCurrent(context);
		alcDestroyContext(context);
		alcCloseDevice(device);

	}

}

bool Sound::enabled = true;

void Sound::setPos(Point *pos) {

	alListener3f(AL_POSITION, pos->x, 0, pos->y);

}

void Sound::setRot(float rot) {

	ALfloat orientation[] = {sinf(rot), 0, cosf(rot), 0, 1, 0};
	alListenerfv(AL_ORIENTATION, orientation);

}

Sound::Sound(char *file) {

	STRING_CAT2(path, "res/audio/", file);

	OggVorbis_File vorbis;
	int result = ov_fopen(path, &vorbis);
	if (result < 0) {

		fprintf(stderr, "Error loading sound '%s': %s\n", file, strerror(errno));
		return;

	}

	vorbis_info *info = ov_info(&vorbis, -1);
	if (info->channels > 1) {

		ov_clear(&vorbis);
		fprintf(stderr, "Error loading sound '%s': More than 1 channel\n", file);

		return;

	}

	ogg_int64_t size = ov_pcm_total(&vorbis, -1) * 2;
	uint8_t *buff = (uint8_t*) malloc(size);

	for (unsigned int i = 0; i < size;) {

		//unsigned int next;

		//if (i + pageSize > size) next = size - i;
		//else next = pageSize;

		int stream;
		i += ov_read(&vorbis, (char*) buff + i, /*next*/pageSize, NULL, 2, 1, &stream);

	}

	alGenBuffers(1, &sound);
	alBufferData(sound, AL_FORMAT_MONO16, (ALvoid*) buff, size, info->rate);

	free(buff);
	ov_clear(&vorbis);

	printf("Loaded sound: '%s'\n", file);

}

Sound::~Sound() {

	if (source) alDeleteSources(1, &source);
	alDeleteBuffers(1, &sound);

}

void Sound::play(bool loop) {

	if (!source) {

		alGenSources(1, &source);
		alSourcei(source, AL_BUFFER, sound);
		alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

	}

	alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	alSourcePlay(source);

}

void Sound::stop() {

	if (source) alSourceStop(source);

}
