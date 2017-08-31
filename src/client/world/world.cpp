#include <GL/gl.h>
#include <stdlib.h>

#include "client.hpp"
#include "gfx/gfx.hpp"
#include "world.hpp"

namespace World {

	GLuint tex;
	unsigned int width;
	unsigned int height;
	int *tiles;

	void init() {

		tex = GFX::loadTexture("world.png");

		tiles = (int*) malloc(4);

		tiles[0] = 0;
		tiles[1] = 1;
		tiles[2] = 2;
		tiles[3] = 3;

		width = 2;
		height = 2;

	}

	void cleanup() {

		glDeleteTextures(1, &tex);

	}

	void draw() {

		if (Client::state != Client::IN_GAME) return;

		for (unsigned int i = 0; i < (width * height); i++) {

			float x = (i % width) * 2.0f;
			float y = (i % height) * 2.0f;

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glTranslatef(x, y, 0);

			glMatrixMode(GL_TEXTURE);
			glPushMatrix();

			glTranslatef(tiles[i] * 0.1f, 0, 0);

			glBindTexture(GL_TEXTURE_2D, tex);
			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(-2.0f, 2.0f);
			glTexCoord2f(0.1f, 1.0f);
			glVertex2f(2.0f, 2.0f);
			glTexCoord2f(0.1f, 0.0f);
			glVertex2f(2.0f, -2.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(-2.0f, -2.0f);

			glEnd();

			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

		}

	}

}
