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
#include <stdio.h>
#include <stddef.h>

#include "button/button.hpp"
#include "client.hpp"
#include "data/data.hpp"
#include "gfx/gfx.hpp"
#include "menu.hpp"
#include "win/win.hpp"

#define ABOUT_LEN 1
#define MAIN_LEN 5
#define SETTINGS_LEN 3

namespace Menu {

	GLuint bg;
	GLuint loading;
	size_t len = MAIN_LEN;
	Button *panel = main;
	bool aboutt;

	void init() {

		Button::init();
		bg = GFX::loadTexture("res/menu.png");
		loading = GFX::loadTexture("res/loading.png");

	}

	void cleanup() {

		glDeleteTextures(1, &bg);
		glDeleteTextures(1, &loading);
		Button::cleanup();

	}

	void tick() {

		if (Client::state == Client::IN_GAME) return;

		for (size_t i = 0; i < len; i++)
			panel[i].tick();

	}

	void draw() {

		static float i = 0.0f;
		i += 0.005f;

		if (Client::state == Client::LOADING) {

			glBindTexture(GL_TEXTURE_2D, loading);
			glBegin(GL_QUADS);

				glTexCoord2f(0.0f, 5.0f);
				glVertex2f(WIN::aspect * -10, 10.0f);
				glTexCoord2f(WIN::aspect * 5, 5.0f);
				glVertex2f(WIN::aspect * 10, 10.0f);
				glTexCoord2f(WIN::aspect * 5, 0.0f);
				glVertex2f(WIN::aspect * 10, -10.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(WIN::aspect * -10, -10.0f);

			glEnd();
			return;

		} else if (Client::state != Client::PAUSED) return;

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();

		glTranslatef(sinf(i), cosf(i), 0.0f);
		glRotatef(sinf(i) * 10, 0.0f, 0.0f, 1.0f);
		glScalef(2.0f, 2.0f, 1.0f);

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

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		for (size_t i = 0; i < len; i++)
			panel[i].draw();

		if (aboutt) GFX::drawText(Data::aboutString, { 0.0f, 3.5f }, 0.7f, true);

		Point p = {(WIN::aspect * -10) + 0.5f, 9.5f};
		GFX::drawText(Data::versionString, p, 1.0f, false);

	}

	void actnBack() {

		panel = main;
		len = MAIN_LEN;
		aboutt = false;

	}

	//about
	Button about[] = {

		Button({0.0f, -3.0f}, 10.0f, "Back", actnBack)

	};

	//main menu
	Button main[] = {

		Button({0.0f, 6.0f}, 10.0f, "Single Player", &actnStart),
		Button({0.0f, 3.0f}, 10.0f, "Multiplayer", &actnStart),
		Button({0.0f, 0.0f}, 10.0f, "Settings", &actnSettings),
		Button({0.0f, -3.0f}, 10.0f, "About", &actnAbout),
		Button({0.0f, -6.0f}, 10.0f, "Quit", &actnQuit)

	};

	void actnStart() {

		Client::setState(Client::LOADING);

	}

	void actnSettings() {

		panel = settings;
		len = SETTINGS_LEN;

	}

	void actnAbout() {

		panel = about;
		len = ABOUT_LEN;
		aboutt = true;

	}

	void actnQuit() {

		Client::running = false;

	}

	//settings
	Button settings[] = {

		Button({0.0f, 3.0f}, 10.0f, "Video", actnBack),
		Button({0.0f, 0.0f}, 10.0f, "Input", actnFullscreen),
		Button({0.0f, -3.0f}, 10.0f, "Back", actnBack)

	};

	void actnFullscreen() {

		WIN::setFullscreen(!WIN::fullscreen);
		settings[0].name = WIN::fullscreen ? (char*) "Fullscreen: true" : (char*) "Fullscreen: false";

	}

}
