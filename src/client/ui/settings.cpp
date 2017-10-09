#include "button/button.hpp"
#include "ui.hpp"
#include "win/win.hpp"

Panel settings;
Panel video;
Panel input;

static bool actnVideo();
static bool actnInput();
static bool actnFullscreen();
static bool actnSetReturn();

void initSettings() {

	settings.buttons.add((void*) new Button({0.0f, 3.0f}, 10.0f, "Video", &actnVideo));
	settings.buttons.add((void*) new Button({0.0f, 0.0f}, 10.0f, "Input", &actnInput));
	settings.buttons.add((void*) new Button({0.0f, -3.0f}, 10.0f, "Back", &actnBack));
	settings.back = &mmenu;

	video.buttons.add((void*) new Button({0.0f, 0.0f}, 10.0f, "Fullscreen: false", &actnFullscreen));
	video.buttons.add((void*) new Button({0.0f, -3.0f}, 10.0f, "Back", &actnBack));
	video.back = &settings;

	input.buttons.add((void*) new Button({0.0f, 0.0f}, 10.0f, NULL, &actnSetReturn));
	input.buttons.add((void*) new Button({0.0f, -3.0f}, 10.0f, "Back", &actnBack));
	input.back = &settings;

}

void cleanupSettings() {

	unsigned int i;

	for (i = 0; i < input.buttons.len; i++)
		delete ((Button*) input.buttons.get(i));

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
