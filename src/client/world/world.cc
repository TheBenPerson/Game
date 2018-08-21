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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "gfx.hh"
#include "net.hh"
#include "timing.hh"
#include "win.hh"
#include "world.hh"

#include "input.hh"

namespace World {

	unsigned int width = 0;
	unsigned int height = 0;
	uint8_t *tiles;
	NodeList listeners;

	float scale = 2;
	float rot;
	Point *pos = NULL;

}

static Timing::Condition condition;
static GFX::texture tex;
static GFX::texture texBG;

static bool tickNet(Packet *packet);
static void draw();

extern "C" {

	bool init() {

		Net::listeners.add((uintptr_t) &tickNet);

		for (;;) {// todo: what if lost connection right before here?

			Net::send(P_GMAP);
			bool result = Timing::waitFor(&condition, 1000);
			if (result) break;

		}

		tex = GFX::loadTexture("world.png");
		texBG = GFX::loadTexture("menu.png");
		GFX::listeners.add((uintptr_t) &draw);

		return true;

	}

	void cleanup() {

		GFX::listeners.rem((uintptr_t) &draw);
		GFX::freeTexture(&texBG);
		GFX::freeTexture(&tex);

		Net::listeners.rem((uintptr_t) &tickNet);

		free(World::tiles);

	}

}

unsigned int World::getIndex(Point *pos) {

	unsigned int x = pos->x + (width / 2.0f);
	unsigned int y = pos->y + (height / 2.0f);

	return (y * World::width) + x;

}

static bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GMAP: {

			// todo: check sizes

			World::width = packet->data[0];
			World::height = packet->data[1];

			unsigned int size = World::width * World::height;

			if (World::tiles) free(World::tiles);

			World::tiles = (uint8_t*) malloc(size);
			memcpy(World::tiles, packet->data + 2, size);

			Timing::signal(&condition);
			printf("Recieved map (%ix%i)\n", World::width, World::height);

		} break;

		case P_SBLK: {

			if (!World::tiles) break;

			unsigned int index = *((uint16_t*) packet->data);
			World::tiles[index] = packet->data[2];

		} break;

		default: return false;

	}

	return true;

}

void draw() {

	if (Client::state != Client::IN_GAME) return;

	if (Input::actions[Input::SECONDARY].state) {

		if (World::scale < 4 && Input::actions[Input::UP].state) World::scale += .25f;
		if (World::scale > .25f && Input::actions[Input::DOWN].state) World::scale -= .25f;

	}

	// draw background

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	if (World::pos) glTranslatef(World::pos->x / 30, World::pos->y / 30, 0);
	glScalef(2 / World::scale, 2 / World::scale, 1);
	glRotatef(-(World::rot * 360) / (M_PI * 2) , 0, 0, 1);
	glTranslatef(-.5f * WIN::aspect, -.5f, 0);

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
	glPopMatrix();

	// draw tiles

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(World::scale, World::scale, 1);
	glRotatef((World::rot * 360) / (M_PI * 2) , 0, 0, 1);
	if (World::pos) glTranslatef(-World::pos->x, -World::pos->y, 0);

	for (unsigned int i = 0; i < (World::width * World::height); i++) {

		Point pos;
		pos.x = (i % World::width) - (World::width / 2.0f) + .5f;
		pos.y = (i / World::width) - (World::height / 2.0f) + .5f;

		Point dim = {1, 1};

		Point dimTex = {11, 1};
		Point frame = {(float) World::tiles[i], 1};

		GFX::drawSprite(tex, &pos, &dim, NULL, &dimTex, &frame);

	}

	for (unsigned int i = 0; i < World::listeners.size; i++) {

		void (*callback)() = (void (*)()) World::listeners[i];
		callback();

	}

	glPopMatrix();

	if (World::pos) {

		char buffer[50];
		sprintf(buffer, "Rot: %.2f\nPos: (%.2f, %.2f)", (World::rot * 360) / (M_PI * 2), World::pos->x, World::pos->y);

		Point pos = {(-10 * WIN::aspect) + 1, 9};
		GFX::drawText(buffer, &pos);

	}

}

