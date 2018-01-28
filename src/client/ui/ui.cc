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

#include <GL/gl.h>
#include <math.h>

#include "audio.hh"
#include "button.hh"
#include "client.hh"
#include "console.hh"
#include "data.hh"
#include "gfx.hh"
#include "input.hh"
#include "win.hh"

static float width = 7;
static float dwidth = width / 2;
static float height = 2;
static float dheight = height / 2;
static float margin = .5f;

static GLuint texBG;
static GLuint texButton;

static Button::Menu *menu;
static Button *selected;
static enum { NORMAL, SELECTED, CLICKED } state = NORMAL;

static void initGL();
static void cleanupGL();
static void tick();
static void draw();

extern "C" {

	char* ui_deps[] = {

		"button.so",
		"gfx.so",
		"audio.so", // to init audio settings
		NULL

	};

	bool init() {

		menu = Button::root;

		Input::listeners.add((void*) &tick);

		GFX::call(&initGL);
		GFX::listeners.add((void*) &draw);

		Audio::play("menu.ogg", true);

		cputs(GREEN, "Loaded module: 'ui.so'");
		return true;

	}

	void cleanup() {

		GFX::listeners.rem((void*) &draw);
		GFX::call(&cleanupGL);

		Input::listeners.rem((void*) &tick);
		cputs(YELLOW, "Unloaded module: 'ui.so'");

	}

}

void initGL() {

	texBG = GFX::loadTexture("menu.png");
	texButton = GFX::loadTexture("button.png");

}

void cleanupGL() {

	glDeleteTextures(1, &texButton);
	glDeleteTextures(1, &texBG);

}

static bool dologic(Button *button, Point *lower, Point *upper) {

	if (button == selected) {

		if (state == SELECTED && Input::actions[Input::A_PRIMARY]) state = CLICKED;
		else {

			if (state == CLICKED) {

				if (button->action.isMenu) {

					if (button->action.menu == (Button::Menu*) -1) menu = menu->parent;
					else menu = button->action.menu;

					selected = NULL;

				} else button->action.callback(button);

				Audio::play("click.ogg");

				state = SELECTED;
				return true;

			}

		}

	}

	if (!(Input::cursor > *lower)) return false;
	if (!(Input::cursor < *upper)) return false;

	if (state != CLICKED) {

		if (selected != button) Audio::play("tick.ogg");
		state = SELECTED;

	}

	selected = button;
	return true;

}

void tick() {

	if (Client::state == Client::IN_GAME) return;

	if (!Input::wasCursor) {

		if (Input::actions[Input::A_EXIT]) {

			if (menu->parent) menu = menu->parent;
			return;

		}

		if (Input::actions[Input::A_UP] || Input::actions[Input::A_DOWN]) {

			Audio::play("tick.ogg");

			if (!selected) {

				bool list = Input::actions[Input::A_UP];

				unsigned int num = 1;
				if (list) {

					num = menu->lists[list].len;
					if (!num) {

						list = !list;
						num = menu->lists[list].len;

					}

				}

				selected = (Button*) menu->lists[list].get(num - 1);

				state = SELECTED;
				return;

			}

			bool list = false;

			NodeList::Node *node = menu->lists[0].find((void*) selected);
			if (!node) {

				node = menu->lists[1].find((void*) selected);
				list = true;

			}

			Button *button = NULL;

			if (Input::actions[Input::A_DOWN]) {

				if (node->next) button = (Button*) node->next->val;
				if (!button) {

					if (menu->lists[!list].len) button = (Button*) menu->lists[!list].get(0);
					else button = (Button*) menu->lists[list].get(0);

				}

			} else {

				if (node->prev) button = (Button*) node->prev->val;
				if (!button) {

					if (menu->lists[!list].len) button = (Button*) menu->lists[!list].get(menu->lists[!list].len - 1);
					else button = (Button*) menu->lists[list].get(menu->lists[!list].len - 1);

				}

			}

			selected = button;

		}

	}

	float offset = ((menu->lists[0].len * (height + margin)) - margin) / 2;
	for (unsigned int i = 0; i < menu->lists[0].len; i++) {

		Point lower = { -dwidth, offset - height - ((height + margin) * i) };
		Point upper = { dwidth, offset - ((height + margin) * i) };

		Button *button = (Button*) menu->lists[0].get(i);
		bool result = dologic(button, &lower, &upper);

		if (result) return;

	}

	for (unsigned int i = 0; i < menu->lists[1].len; i++) {

		Button *button = (Button*) menu->lists[1].get(i);

		Point lower = { button->pos->x - dwidth, button->pos->y - dheight };
		Point upper = { button->pos->x + dwidth, button->pos->y + dheight };

		dologic(button, &lower, &upper);

	}

}

static void drawButton(Button *button) {

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	if (button == selected) glTranslatef(0, -state * (1 / 8.0f), 0);

		glBindTexture(GL_TEXTURE_2D, texButton);
		glBegin(GL_QUADS);

			glTexCoord2f(0, 5.0f / 8.0f);
			glVertex2f(-dwidth, dheight);
			glTexCoord2f(1, 5.0f / 8.0f);
			glVertex2f(-dwidth + 1, dheight);
			glTexCoord2f(1, 4.0f / 8.0f);
			glVertex2f(-dwidth + 1, -dheight);
			glTexCoord2f(0, 4.0f / 8.0f);
			glVertex2f(-dwidth, -dheight);

			glTexCoord2f(0, 1);
			glVertex2f(-dwidth + 1, dheight);
			glTexCoord2f(width, 1);
			glVertex2f(dwidth - 1, dheight);
			glTexCoord2f(width, 7.0f / 8.0f);
			glVertex2f(dwidth - 1, -dheight);
			glTexCoord2f(0, 7.0f / 8.0f);
			glVertex2f(-dwidth + 1, -dheight);

			glTexCoord2f(1, 5.0f / 8.0f);
			glVertex2f(dwidth - 1, dheight);
			glTexCoord2f(0, 5.0f / 8.0f);
			glVertex2f(dwidth, dheight);
			glTexCoord2f(0, 4.0f / 8.0f);
			glVertex2f(dwidth, -dheight);
			glTexCoord2f(1, 4.0f / 8.0f);
			glVertex2f(dwidth - 1, -dheight);

		glEnd();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	GFX::drawText(button->name, {0, 0}, 1, true);

}

void draw() {

	if (Client::state == Client::IN_GAME) return;
	// fix this section w/ better logic

	static float i = 0;
	i += 0.005f;

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(sinf(i), cosf(i), 0);
	glRotatef(sinf(i) * 10, 0, 0, 1);
	glScalef(2, 2, 1);

		// this block causes a memory leak somehow
		glBindTexture(GL_TEXTURE_2D, texBG);
		glBegin(GL_QUADS);

			glTexCoord2f(0, 1);
			glVertex2f(WIN::aspect * -10, 10);
			glTexCoord2f(WIN::aspect, 1);
			glVertex2f(WIN::aspect * 10, 10);
			glTexCoord2f(WIN::aspect, 0);
			glVertex2f(WIN::aspect * 10, -10);
			glTexCoord2f(0, 0);
			glVertex2f(WIN::aspect * -10, -10);

		glEnd();
		// end block

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// draw auto positioned buttons
	for (unsigned int i = 0; i < menu->lists[0].len; i++) {

		Button *button = (Button*) menu->lists[0].get(i);

		glPushMatrix();
		float offset = (((menu->lists[0].len * (height + margin)) - margin) / 2) - dheight;
		glTranslatef(0, offset - (i * (height + margin)), 0);

			drawButton(button);

		glPopMatrix();

	}

	// draw manually positioned button
	for (unsigned int i = 0; i < menu->lists[1].len; i++) {

		Button *button = (Button*) menu->lists[1].get(i);

		glPushMatrix();
		glTranslatef(button->pos->x, button->pos->y, 0);

			drawButton(button);

		glPopMatrix();

	}

	GFX::drawText(Data::versionString, {(-10 * WIN::aspect) + 1, 9});

}
