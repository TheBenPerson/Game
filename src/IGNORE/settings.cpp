/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>
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

#include "audio.hpp"
#include "button.hpp"
#include "ui.hpp"
#include "win.hpp"

Panel settings;
Panel video;
Panel audio;
Panel input;

static bool actnVideo();
static bool actnAudio();
static bool actnInput();
static bool actnFullscreen();
static bool actnSetReturn();

void initSettings() {

	settings.buttons.add((void*) new Button({0, 3}, 10, "Video", &actnVideo));
	settings.buttons.add((void*) new Button({0, 1.5f}, 10, "Audio", &actnAudio));
	settings.buttons.add((void*) new Button({0, -1.5f}, 10, "Input", &actnInput));
	settings.buttons.add((void*) new Button({0, -3}, 10, "Back", &actnBack));
	settings.back = &mmenu;

	video.buttons.add((void*) new Button({0, 0}, 10, "Fullscreen: false", &actnFullscreen));
	video.buttons.add((void*) new Button({0, -3}, 10, "Back", &actnBack));
	video.back = &settings;

	audio.buttons.add((void*) new Button({0, 0}, 10, NULL, &actnToggleAudioc));
	audio.buttons.add((void*) new Button({0, -3}, 10, "Back", &actnBack));
	audio.back = &settings;

	input.buttons.add((void*) new Button({0, 0}, 10, NULL, &actnSetReturn));
	input.buttons.add((void*) new Button({0, -3}, 10, "Back", &actnBack));
	input.back = &settings;

}

void cleanupSettings() {

	unsigned int i;

	for (i = 0; i < input.buttons.len; i++)
		delete ((Button*) input.buttons.get(i));

	for (i = 0; i < audio.buttons.len; i++)
		delete ((Button*) audio.buttons.get(i));

	for (i = 0; i < video.buttons.len; i++)
		delete ((Button*) video.buttons.get(i));

	for (i = 0; i < settings.buttons.len; i++)
		delete ((Button*) settings.buttons.get(i));

}

bool actnVideo() {

	Button* button = (Button*) video.buttons.get(0);

	if (WIN::fullscreen) button->name = "Fullscreen: true";
	else button->name = "Fullscreen: false";

	setPanel(&video);
	return true;

}

bool actnAudio() {

	Button* button = (Button*) audio.buttons.get(0);

	if (Audio::enabled) button->name = "Audio: enabled";
	else button->name = "Audio: disabled";

	setPanel(&audio);
	return true;

}

bool actnInput() {

	((Button*) input.buttons.get(0))->name = "Action: Return";
	setPanel(&input);
	return true;

}

bool actnFullscreen() {

	WIN::setFullscreen(!WIN::fullscreen);

	Button* button = (Button*) video.buttons.get(0);

	if (WIN::fullscreen) button->name = "Fullscreen: true";
	else button->name = "Fullscreen: false";

	return false;

}

bool actnSetReturn() {

	((Button*) input.buttons.get(0))->name = "Not implemented (yet)";
	return false;

}
