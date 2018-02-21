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
#include <string.h>

#include "client.hh"
#include "console.hh"
#include "gfx.hh"
#include "input.hh"
#include "net.hh"
#include "win.hh"
#include "world.hh"

namespace World {

	GLuint tex;
	unsigned int width = 0;
	unsigned int height = 0;
	Tile *tiles;
	NodeList listeners;

	float rot;
	Point pos = {0, 0};

}

float scale = 2;
static float speed = 0.075f;

static bool tickNet(Packet *packet);
static void tickInput();
static void tick();
static void initGL();
static void cleanupGL();
static void draw();

extern "C" {

	bool init() {

		Net::listeners.add((void*) tickNet);
		Net::send(P_GMAP);

		Input::listeners.add((void*) &tickInput);

		GFX::call(&initGL);
		GFX::listeners.add((void*) &draw);

		cputs(GREEN, "Loaded module: 'world.so'");

		return true;

	}

	void cleanup() {

		GFX::listeners.rem((void*) &draw);
		GFX::call(&cleanupGL);

		Input::listeners.rem((void*) &tickInput);

		free(World::tiles);
		cputs(YELLOW, "Unloaded module: 'world.so'");

	}

}

static bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_GMAP: {

			// todo: check sizes

			World::width = packet->raw[1];
			World::height = packet->raw[2];

			unsigned int size = sizeof(Tile) * World::width * World::height;
			World::tiles = (Tile*) malloc(size);
			memcpy(World::tiles, packet->raw + 3, size);

			printf("Recieved map (%ix%i)\n", World::width, World::height);

		} break;

		case P_SBLK: {

			unsigned int index = *((uint16_t*) packet->data);
			World::tiles[index].id = (Tile::type) packet->data[2];

		} break;

		default: return false;

	}

	return true;

}

// callback
void tickInput() {

	if (Input::wasCursor) World::rot = (Input::cursor.x / (10 * WIN::aspect)) * 2 * M_PI;
	else if (Input::actions[Input::A_EXIT]) Client::state = Client::PAUSED;

}

// per draw
void tick() {

	if (Input::actions[Input::A_SECONDARY]) {

		if (Input::actions[Input::A_UP]) scale += speed;
		else if (Input::actions[Input::A_DOWN]) scale -= speed;

		return;

	}

	if (Input::actions[Input::A_PRIMARY]) {

		unsigned int x = World::pos.x + (World::width / 2.0f);
		unsigned int y = (World::height / 2.0f) - World::pos.y;

		unsigned int index = (y * World::width) + x;

		if (World::tiles[index].id != Tile::SAND) {

			World::tiles[index].id = Tile::SAND;
			uint8_t data[] = {P_SBLK, x, y, Tile::SAND};

			Packet packet;
			packet.raw = data;
			packet.size = 4;

			Net::send(&packet);

		}

	}

	if (Input::actions[Input::A_UP]) {


		World::pos.x += sinf(World::rot) * speed;
		World::pos.y += cosf(World::rot) * speed;

	}

	if (Input::actions[Input::A_DOWN]) {

		World::pos.x -= sinf(World::rot) * speed;
		World::pos.y -= cosf(World::rot) * speed;

	}

	if (Input::actions[Input::A_LEFT]) {

		World::pos.x += sinf(World::rot - M_PI_2) * speed;
		World::pos.y += cosf(World::rot - M_PI_2) * speed;

	}

	if (Input::actions[Input::A_RIGHT]) {

		World::pos.x -= sinf(World::rot - M_PI_2) * speed;
		World::pos.y -= cosf(World::rot - M_PI_2) * speed;

	}

}

void initGL() {

	World::tex = GFX::loadTexture("world.png");

}

void cleanupGL() {

	glDeleteTextures(1, &World::tex);

}

void draw() {

	if (Client::state != Client::IN_GAME) return;

	glPushMatrix();
	glScalef(scale, scale, 1);
	glRotatef((World::rot * 360) / (M_PI * 2) , 0, 0, 1);
	glTranslatef(-World::pos.x, -World::pos.y, 0);

	for (unsigned int i = 0; i < (World::width * World::height); i++) {

		float x = (i % World::width) - (World::width / 2.0f);
		float y = (World::height - 1) - (i / World::width) - (World::height / 2.0f);

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

	for (unsigned int i = 0; i < World::listeners.len; i++) {

		void (*callback)() = (void (*)()) World::listeners.get(i);
		callback();

	}

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, NULL);
	glBegin(GL_LINES);

	glVertex2f(-1, 0);
	glVertex2f(1, 0);

	glVertex2f(0, -1);
	glVertex2f(0, 1);

	glEnd();

	char buffer[50];
	sprintf(buffer, "Rot: %f\nPos: (%f, %f)", (World::rot * 360) / (M_PI * 2), World::pos.x, World::pos.y);

	Point pos = {(-10 * WIN::aspect) + 1, 9};
	GFX::drawText(buffer, &pos);

	tick();

}

