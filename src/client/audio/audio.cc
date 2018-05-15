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

#include <alsa/asoundlib.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <vorbis/vorbisfile.h>

#include "audio.hh"
#include "client.hh"
#include "console.hh"
#include "timing.hh"

#define PAGE_SIZE 4096 // page aligned

typedef struct {

	char *path;
	bool loop;

} Args;

static unsigned int threads = 0;

static void toggle(Button *button) {

	Audio::enabled = !Audio::enabled;

	if (Audio::enabled) {

		button->name = "Audio: Enabled";
		puts("Enabled audio");

	} else {

		button->name = "Audio: Disabled";
		puts("Disabled Audio");

	}

}

extern "C" {

	bool init() {

		Client::config.set("audio.enabled", true);
		Client::config.load("audio.cfg");

		Audio::enabled = (bool) Client::config.get("audio.enabled");
		if (Audio::enabled) puts("Audio enabled");
		else puts("Audio enabled");

		Button::Action action;
		action.menu = NULL;
		action.isMenu = true;

		Button::Menu *menu = (new Button("Audio", &action, Client::settings))->action.menu;

		action.callback = &toggle;
		action.isMenu = false;

		if (Audio::enabled) new Button("Audio: Enabled", &action, menu);
		else new Button("Audio: Disabled", &action, menu);

		cputs(GREEN, "Loaded module: 'audio.so'");
		return true;

	}

	void cleanup() {

		Audio::enabled = false;
		while (threads) {}

		cputs(YELLOW, "Unloaded module: 'audio.so'");

	}

}

static void* play(void *arg) {

	threads++;

	Args *args = (Args*) arg;

	OggVorbis_File sound;
	int result = ov_fopen(args->path, &sound);

	if (result < 0) {

		fprintf(stderr, "Error opening file '%s': %s\n", args->path, strerror(errno));

		bool loop = args->loop;
		args->loop = !loop; // signal parent

		threads--;
		return NULL;

	}

	bool loop = args->loop;
	args->loop = !loop; // signal parent

	snd_pcm_t *pcm;
	result = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);
	if (result < 0) {

		fprintf(stderr, "Error opening device: %s\n", snd_strerror(result));
		ov_clear(&sound);

		threads--;
		return NULL;

	}

	snd_pcm_hw_params_t *hw_params;
	snd_pcm_hw_params_malloc(&hw_params);
	snd_pcm_hw_params_any(pcm, hw_params);

	snd_pcm_hw_params_set_access(pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE); // signed 16 bit little-endian

	vorbis_info *info = ov_info(&sound, -1);

	unsigned int rate = (unsigned int) info->rate;
	snd_pcm_hw_params_set_channels(pcm, hw_params, info->channels);
	snd_pcm_hw_params_set_rate_near(pcm, hw_params, &rate, 0);

	snd_pcm_hw_params(pcm, hw_params);
	snd_pcm_hw_params_free(hw_params);

	snd_pcm_prepare(pcm);

	uint8_t buffer[PAGE_SIZE];

	while (Audio::enabled) {

		int stream;

		int result = ov_read(&sound, (char*) &buffer, PAGE_SIZE, 0, 2, 1, &stream);
		if (!result) {

			ov_raw_seek(&sound, 0);
			if (!loop) break;

			continue;

		}

		snd_pcm_writei(pcm, &buffer, result / (2 * info->channels));

	}

	snd_pcm_close(pcm);
	ov_clear(&sound);

	threads--;
	return NULL;

}

namespace Audio {

	bool enabled;

	void play(char *file, bool loop) {

		if (!enabled) return;

		char *path = new char[11 + strlen(file)]; // todo: create auto-mallocing sprintf
		sprintf(path, "res/audio/%s", file);

		Args args;
		args.path = path;
		args.loop = loop;

		Timing::createThread(&::play, (void*) &args);
		while (args.loop == loop) {}

		delete[] path;

	}

}
