/*

Game Development Build
https://github.com/TheBenPerson/Game

Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <GL/gl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>

#include "button.hpp"
#include "client.hpp"
#include "console.hpp"
#include "data.hpp"
#include "gfx.hpp"
#include "input.hpp"
#include "timing.hpp"
#include "ui.hpp"
#include "win.hpp"

GLuint bg;
Panel *panel = &mmenu;
Timing::mutex m = MTX_DEFAULT;

Panel mmenu;
Panel about;

void initGL();
void cleanupGL();
void tick();
void draw();
void setPanel(Panel* panel);

bool actnStart();
bool actnSettings();
bool actnAbout();
bool actnQuit();

extern "C" {

	char* depends[] = {

		"gfx.so",
		NULL

	};

	bool init() {

		mmenu.buttons.add((void*) new Button({0.0f, 6.0f}, 10.0f, "Single Player", &actnStart));
		mmenu.buttons.add((void*) new Button({0.0f, 3.0f}, 10.0f, "Multiplayer", &actnStart));
		mmenu.buttons.add((void*) new Button({0.0f, 0.0f}, 10.0f, "Settings", &actnSettings));
		mmenu.buttons.add((void*) new Button({0.0f, -3.0f}, 10.0f, "About", &actnAbout));
		mmenu.buttons.add((void*) new Button({0.0f, -6.0f}, 10.0f, "Quit", &actnQuit));
		mmenu.back = NULL;

		initSettings();

		about.buttons.add((void*) new Button({0.0f, -3.0f}, 10.0f, "Back", actnBack));
		about.back = &mmenu;

		Input::listeners.add((void*) &tick);

		GFX::call(&initGL);
		GFX::listeners.add((void*) &draw);

		cputs("Loaded module: 'ui.so'");

		return true;

	}

	void cleanup() {

		GFX::listeners.rem((void*) &draw);
		GFX::call(&cleanupGL);

		Input::listeners.rem((void*) &tick);

		cleanupSettings();

		unsigned int i;

		for (i = 0; i < mmenu.buttons.len; i++)
			delete ((Button*) mmenu.buttons.get(i));

		for (i = 0; i < about.buttons.len; i++)
			delete ((Button*) about.buttons.get(i));

		cputs("Unoaded module: 'ui.so'", RED);

	}

}

void initGL() {

	Button::init();

	bg = GFX::loadTexture("menu.png");
	loading = GFX::loadTexture("loading.png");

}

void cleanupGL() {

	glDeleteTextures(1, &bg);
	glDeleteTextures(1, &loading);

	Button::cleanup();

}

void tick() {

	if (Client::state == Client::IN_GAME) return;

	if (Input::actions[Input::A_EXIT])
		if (panel->back) setPanel(panel->back);

	for (unsigned int i = 0; i < panel->buttons.len; i++)
		if (((Button*) panel->buttons.get(i))->tick()) i = 0; // tick all if something changed

}

void draw() {

	if (Client::state == Client::IN_GAME) return;
	// fix this section w/ better logic

	static float i = 0.0f;
	i += 0.005f;

	if (Client::state == Client::LOADING) drawLoading();
	else {

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();

		glTranslatef(sinf(i), cosf(i), 0.0f);
		glRotatef(sinf(i) * 10, 0.0f, 0.0f, 1.0f);
		glScalef(2.0f, 2.0f, 1.0f);

		// this block causes a memory leak somehow
		glBindTexture(GL_TEXTURE_2D, bg);
		glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(WIN::aspect * -10, 10.0f);
			glTexCoord2f(WIN::aspect, 1.0f);
			glVertex2f(WIN::aspect * 10, 10.0f);
			glTexCoord2f(WIN::aspect, 0.0f);
			glVertex2f(WIN::aspect * 10, -10.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(WIN::aspect * -10, -10.0f);

		glEnd();
		// end block

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		if (panel == &about) GFX::drawText(Data::aboutString, { 0.0f, 3.5f }, 0.7f, true);

		Point p = {(WIN::aspect * -10) + 0.5f, 9.5f};
		GFX::drawText(Data::versionString, p);

	}

	Timing::lock(&m);

	for (unsigned int i = 0; i < panel->buttons.len; i++)
		((Button*) panel->buttons.get(i))->draw();

	Timing::unlock(&m);

}

void setPanel(Panel* panel) {

	Timing::lock(&m);
	::panel = panel;
	Timing::unlock(&m);

}

bool actnBack() {

	setPanel(panel->back);
	return true;

}

bool actnStart() {

	Client::setState(Client::LOADING);
	setPanel(&cancel);

	return true;

}

bool actnSettings() {

	setPanel(&settings);
	return true;

}

bool actnAbout() {

	setPanel(&about);
	return true;

}

bool actnQuit() {

	killpg(NULL, SIGINT);
	return true;

}
