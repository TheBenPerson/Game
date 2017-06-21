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
#include "timing/timing.hpp"
#include "win/win.hpp"

namespace Menu {

	GLuint bg;
	Button** panel = main;
	Timing::mutex m = MTX_DEFAULT;

	void init() {

		Button::init();

		bg = GFX::loadTexture("menu.png");
		loading = GFX::loadTexture("loading.png");

	}

	void cleanup() {

		glDeleteTextures(1, &bg);
		glDeleteTextures(1, &loading);
		Button::cleanup();

	}

	void tick() {

		if (Client::state == Client::IN_GAME) return;

		for (size_t i = 1; panel[i]; i++)
			if (panel[i]->tick()) i = 1; //tick all if something changed

	}

	void draw() {

		static float i = 0.0f;
		i += 0.005f;

		if (Client::state == Client::LOADING) drawLoading();
		else {

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

			if (panel == about) GFX::drawText(Data::aboutString, { 0.0f, 3.5f }, 0.7f, true);

			Point p = {(WIN::aspect * -10) + 0.5f, 9.5f};
			GFX::drawText(Data::versionString, p);

		}

		Timing::lock(&m);

		for (size_t i = 1; panel[i]; i++)
			panel[i]->draw();

		Timing::unlock(&m);


	}

	void setPanel(Button* panel[]) {

		Timing::lock(&m);
		Menu::panel = panel;
		Timing::unlock(&m);

	}

	bool actnBack() {

		if (panel[0]) setPanel((Button**) panel[0]);
		return true;

	}

	//about
	Button* about[] = {

		(Button*) main,
		new Button({0.0f, -3.0f}, 10.0f, "Back", actnBack),
		NULL

	};

	//main menu
	Button* main[] = {

		NULL,
		new Button({0.0f, 6.0f}, 10.0f, "Single Player", &actnStart),
		new Button({0.0f, 3.0f}, 10.0f, "Multiplayer", &actnStart),
		new Button({0.0f, 0.0f}, 10.0f, "Settings", &actnSettings),
		new Button({0.0f, -3.0f}, 10.0f, "About", &actnAbout),
		new Button({0.0f, -6.0f}, 10.0f, "Quit", &actnQuit),
		NULL

	};

	bool actnStart() {

		Client::setState(Client::LOADING);
		setPanel(bCancel);

		return true;

	}

	bool actnSettings() {

		setPanel(settings);
		return true;

	}

	bool actnAbout() {

		setPanel(about);
		return true;

	}

	bool actnQuit() {

		Client::running = false;
		return true;

	}

}
