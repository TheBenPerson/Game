#include "button/button.hpp"
#include "ui.hpp"
#include "win/win.hpp"

Button* settings[] = {

	(Button*) mmenu,
	new Button({0.0f, 3.0f}, 10.0f, "Video", &actnVideo),
	new Button({0.0f, 0.0f}, 10.0f, "Input", &actnInput),
	new Button({0.0f, -3.0f}, 10.0f, "Back", &actnBack),
	NULL

};

bool actnVideo() {

	if (WIN::fullscreen) video[1]->name = "Fullscreen: true";
	else video[1]->name = "Fullscreen: false";

	setPanel(video);
	return true;

}

bool actnInput() {

	input[1]->name = "Action: Return";
	setPanel(input);
	return true;

}

Button* video[] = {

	(Button*) settings,
	new Button({0.0f, 0.0f}, 10.0f, "Fullscreen: false", &actnFullscreen),
	new Button({0.0f, -3.0f}, 10.0f, "Back", &actnBack),
	NULL

};

bool actnFullscreen() {

	WIN::setFullscreen(!WIN::fullscreen);
	return false;

}

Button* input[] = {

	(Button*) settings,
	new Button({0.0f, 0.0f}, 10.0f, NULL, &actnSetReturn),
	new Button({0.0f, -3.0f}, 10.0f, "Back", &actnBack),
	NULL

};

bool actnSetReturn() {

	input[1]->name = "Not implemented (yet)";
	return false;

}
