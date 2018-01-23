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

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "point.hh"
#include "win.hh"
#include "world.hh"

namespace World {

	GLuint tex;
	unsigned int width = 0;
	unsigned int height = 0;
	Tile *tiles;

	float scale = 1;
	float rot;

}

static Point pos = {0, 0};

static void draw() {

	if (Client::state != Client::IN_GAME) return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(World::scale, World::scale, 1);

	glRotatef((World::rot * 360) / (M_PI * 2) , 0, 0, 1);
	glTranslatef(-pos.x, -pos.y, 0);

	for (unsigned int i = 0; i < (World::width * World::height); i++) {

		float x = (i % World::width) - (World::width / 2.0f);
		float y = (i / World::height) - (World::height / 2.0f);

		glPushMatrix();
		glTranslatef(x, y, 0);

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();

		glTranslatef(World::tiles[i].id * 0.1f, 0, 0);

		glBindTexture(GL_TEXTURE_2D, World::tex);
		glBegin(GL_QUADS);

		glTexCoord2f(0, 1);
		glVertex2f(0, 1);
		glTexCoord2f(.1f, 1);
		glVertex2f(1, 1);
		glTexCoord2f(.1f, 0);
		glVertex2f(1, 0);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);

		glEnd();

		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	}

	glPopMatrix();

	glBegin(GL_LINES);

	glVertex2f(-5, 0);
	glVertex2f(5, 0);

	glVertex2f(0, -5);
	glVertex2f(0, 5);

	glEnd();

	char buffer[50];
	sprintf(buffer, "Rot: %f\nPos: (%f, %f)", (World::rot * 360) / (M_PI * 2), pos.x, pos.y);

	GFX::drawText(buffer, {(-10 * WIN::aspect) + 1, 9});

}

static float step = 0.05f;

static void tick() {

	if (Client::state != Client::IN_GAME) return;

	if (Input::wasCursor) {

		World::rot = (Input::cursor.x / (10 * WIN::aspect)) * 2 * M_PI;
		return;
	}

	if (Input::actions[Input::A_EXIT]) {

		Client::state = Client::PAUSED;
		return;

	}

	if (Input::actions[Input::A_ACTION]) {

		if (Input::actions[Input::A_UP]) World::scale += step;
		if (Input::actions[Input::A_DOWN]) World::scale -= step;
		return;

	}

	if (Input::actions[Input::A_UP]) {


		pos.x += sinf(World::rot) * step;
		pos.y += cosf(World::rot) * step;

	}

	if (Input::actions[Input::A_DOWN]) {

		pos.x -= sinf(World::rot) * step;
		pos.y -= cosf(World::rot) * step;

	}

	if (Input::actions[Input::A_LEFT]) {

		pos.x += sin(World::rot - M_PI_2) * step;
		pos.y += cos(World::rot - M_PI_2) * step;

	}

	if (Input::actions[Input::A_RIGHT]) {

		pos.x -= sin(World::rot - M_PI_2) * step;
		pos.y -= cos(World::rot - M_PI_2) * step;

	}

}

static void initGL() {

	World::tex = GFX::loadTexture("world.png");

}

static void cleanupGL() {

	glDeleteTextures(1, &World::tex);

}

static bool netTick(Packet *packet) {

	switch (packet->raw[0]) {

		case P_SMAP:

			// todo: check sizes

			World::width = packet->raw[1];
			World::height = packet->raw[2];

			size_t size = sizeof(Tile) * World::width * World::height;
			World::tiles = (Tile*) malloc(size);
			memcpy(World::tiles, packet->raw + 3, size);

			printf("Recieved map (%ix%i)\n", World::width, World::height);

		break;

		default: return false;

	}

	return true;

}

extern "C" {

	char* depends[] = {

		"net.so",
		"input.so",
		"gfx.so",
		NULL

	};

	bool init() {

		Net::listeners.add((void*) netTick);
		Net::send(P_GMAP);

		Input::listeners.add((void*) &tick);

		GFX::call(&initGL);
		GFX::listeners.add((void*) &draw);

		cputs(GREEN, "Loaded module: 'world.so'");

		return true;

	}

	void cleanup() {

		GFX::listeners.rem((void*) &draw);
		GFX::call(&cleanupGL);

		Input::listeners.rem((void*) &tick);

		free(World::tiles);
		cputs(YELLOW, "Unloaded module: 'world.so'");

	}

}
