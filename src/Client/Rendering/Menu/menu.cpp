/*

Copyright (C) 2016-2017 Ben Stockett

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
#include "Button/button.hpp"
#include "client.hpp"
#include "menu.hpp"
#include "Rendering/rendering.hpp"
#include "Util/NodeList/nodelist.hpp"
#include "XClient/xclient.hpp"

#define MAIN_LEN 4
#define SETTINGS_LEN 2

bool Menu::active = true;
bool Menu::inGame = false;

GLuint Menu::background;
size_t Menu::menuLen = 4;
Button* Menu::panel = mainMenu;

Button Menu::mainMenu[] = {

	Button({0.0f, 4.5f}, 10.0f, "Single Player", NULL),
	Button({0.0f, 1.5f}, 10.0f, "Multiplayer", NULL),
	Button({0.0f, -1.5f}, 10.0f, "Settings", settingsProc),
	Button({0.0f, -4.5f}, 10.0f, "Quit", quitProc)

};

void Menu::settingsProc() {

	panel = settings;
	menuLen = SETTINGS_LEN;

}

void Menu::quitProc() {

	Client::running = false;

}

Button Menu::settings[] = {

	Button({0.0f, 0.0f}, 10.0f, "Fullscreen: false", fullscreenProc),
	Button({0.0f, -3.0f}, 10.0f, "Back", backProc)

};

void Menu::backProc() {

	panel = mainMenu;
	menuLen = MAIN_LEN;

}

void Menu::fullscreenProc() {

	XClient::setFullscreen(!XClient::fullscreen);
	settings[0].name = XClient::fullscreen ? (char*) "Fullscreen: true" : (char*) "Fullscreen: false";

}

void Menu::init() {

	Button::init();
	background = Rendering::loadTexture("res/background.png");

}

void Menu::cleanup() {

	glDeleteTextures(1, &background);

	Button::cleanup();

}

void Menu::draw() {

	static float i = 0.0f;
	i += 0.005f;

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();

	glTranslatef(sinf(i), cosf(i), 0.0f);
	glRotatef(sinf(i) * 10, 0.0f, 0.0f, 1.0f);
	glScalef(2.0f, 2.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, background);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(XClient::aspect * -10, 10.0f);
		glTexCoord2f(XClient::aspect, 1.0f);
		glVertex2f(XClient::aspect * 10, 10.0f);
		glTexCoord2f(XClient::aspect, 0.0f);
		glVertex2f(XClient::aspect * 10, -10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(XClient::aspect * -10, -10.0f);

	glEnd();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	for (size_t i = 0; i < menuLen; i++)
		panel[i].draw();

}
