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

#include <errno.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <math.h>
#include <png.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include "client.hh"
#include "config.hh"
#include "console.hh"
#include "gfx.hh"
#include "point.hh"
#include "timing.hh"
#include "win.hh"

namespace GFX {

	NodeList listeners;
	unsigned int frame;

}

static 	Timing::thread t;
static Timing::mutex m = MTX_DEFAULT;
static void* (*callback)(void*) = NULL;
static void* arg;
static bool running = true;
static GLuint font;

static void* threadMain(void*);
static void glInit();
static GFX::texture loadTexture(char *name);
static void freeTexture(GFX::texture *tex);
static void draw();

extern "C" {

	bool init() {

		Client::config.set("gfx.fps", (void*) 60);
		Client::config.set("gfx.res", (void*) "default");
		Client::config.load("gfx.cfg");

		volatile int8_t result = -1; //  volatile because reasons
		t = Timing::createThread(threadMain, (void*) &result);

		while (result == -1) {}

		if (!result) {

			Timing::waitFor(t);
			return false;

		}

		cputs(GREEN, "Loaded module: 'gfx.so'");
		return true;

	}

	void cleanup() {

		running = false;
		Timing::waitFor(t);

		cputs(YELLOW, "Unloaded module: 'gfx.so'");

	}

}

// global functions

namespace GFX {

	void* call(void* (*function)(void*), void *arg) {

		Timing::lock(&m);

		callback = function;
		::arg = arg;

		while (callback) {} // TODO: use condition?

		Timing::unlock(&m);

		return ::arg;

	}

	void drawText(char *text, Point *point, float size, bool center) {

		unsigned int width = 16;
		unsigned int height = 6;

		Point dimTex = {1.0f / width, 1.0f / height};
		Point dimChar = dimTex * size * 8;
		float spacing = 0;

		size_t len = strlen(text);

		Point pos = *point;
		float dx = 0;

		if (center) {

			char *start = text;
			char *end;

			unsigned int lines = 0;
			size_t best = 0;

			// get number of lines and max width
			for (;;) {

				lines++;

				size_t tlen = 0;

				end = strchr(start, '\n');
				if (!end) {

					// find length of last line
					tlen = (text + len) - start - lines;
					if (len > best) best = tlen;

					break;

				}

				// find length of line
				tlen = end - start;
				if (tlen > best) best = tlen;

				start = end + 1;

			}

			dx = ((dimChar.x + spacing) * best) / 2;

			pos.x -= dx;
			pos.y -= ((lines * dimChar.y) / 2) - dimChar.y;

		}

		for (unsigned int i = 0; i < len; i++) {

			if (text[i] == '\n') {

				pos.y -= dimChar.y;
				pos.x = point->x - dx;
				continue;

			}

			Point offset;
			offset.x = ((text[i] - 32) % width) * dimTex.x;
			offset.y = -(((text[i] - 32) / width) * dimTex.y);

			glPushMatrix();
			glTranslatef(pos.x, pos.y, 0);
			glScalef(dimChar.x, dimChar.y, 1);

			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			glTranslatef(offset.x, offset.y - dimTex.y, 0);
			glScalef(dimTex.x, dimTex.y, 1);

			glBindTexture(GL_TEXTURE_2D, font);
			glBegin(GL_QUADS);

			glTexCoord2f(1, 1);
			glVertex2f(1, 0);
			glTexCoord2f(0, 1);
			glVertex2f(0, 0);
			glTexCoord2f(0, 0);
			glVertex2f(0, -1);
			glTexCoord2f(1, 0);
			glVertex2f(1, -1);

			glEnd();
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			pos.x += dimChar.x + spacing;

		}

	}

	texture loadTexture(char *name) {

		return (GLuint) call((void* (*)(void*)) &::loadTexture, (void*) name);

	}

	void freeTexture(texture *tex) {

		call((void* (*)(void*)) &::freeTexture, (void*) tex);

	}

}

// static functions

void* threadMain(void* result) {

	if (!WIN::initContext()) {

		*((int8_t *) result) = 0; // tell main thread init failed
		return NULL;

	}

	font = loadTexture("font.png");
	glInit(); // initialize OpenGL

	WIN::showWindow(); // display window
	*((int8_t *) result) = 1; // tell main thread init succeeded

	if (WIN::vSync) while (running) draw();
	else Timing::doInterval(&draw, (time_t) Client::config.get("gfx.fps")->val, false, &running);

	glDeleteTextures(1, &font);
	WIN::cleanupContext();

	return NULL;

}


void glInit() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

}

GFX::texture loadTexture(char *name) { // consider strlen instead of constants

	char *res = (char*) Client::config.get("gfx.res")->val;
	size_t len = strlen(name);

	char *buf = (char*) malloc(12 + strlen(res) + 1 + len + 1);
	sprintf(buf, "res/texture/%s/%s", res, name); // strcpy might be faster

	FILE *file = fopen(buf, "r");
	free(buf);

	if (!file) {

		buf = (char*) malloc(20 + len + 1);
		sprintf(buf, "res/texture/default/%s", name);

		file = fopen(buf, "r");
		free(buf);

		if (!file) {

			fprintf(stderr, "Error loading texture: '%s' (%s)\n", name, strerror(errno));
			return NULL;

		}

	}

	unsigned char sig[8];
	fread(sig, 8, 1, file);

	if (png_sig_cmp(sig, 0, 8)) {

		fprintf(stderr, "Error loading texture: '%s' (Invalid PNG file)\n", name);
		fclose(file);

		return 0;

	}

	png_struct *png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_info *info = png_create_info_struct(png);

	png_init_io(png, file);
	png_set_sig_bytes(png, 8);

	png_read_info(png, info);

	png_uint_32 width = png_get_image_width(png, info);
	png_uint_32 height = png_get_image_height(png, info);

	png_byte channels = png_get_channels(png, info);
	if (channels < 3) fprintf(stderr, "Warning: texture '%s' has < 3 channels\n", name);

	png_byte* image = (png_byte*) malloc(width * height * channels);

	png_uint_32 i = height - 1;

	for (;;) {

		png_read_row(png, image + (i * width * channels), NULL);
		if (!i) break;

		i--;

	}

	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, (channels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);

	free(image);
	png_destroy_read_struct(&png, &info, NULL);
	fclose(file);

	printf("Loaded texture: '%s' (%ix%i)\n", name, width, height);
	return tex;

}

void freeTexture(GFX::texture *tex) {

	glDeleteTextures(1, tex);

}

void draw() {

	if (callback) {

		arg = callback(arg);
		callback = NULL; // TODO: mutex?

	}

	if (WIN::resized) {

		glViewport(0, 0, WIN::width, WIN::height);
		double aspect = (double) WIN::width / (double) WIN::height;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-10 * aspect, 10 * aspect, -10, 10, -10, 10);

		WIN::resized = false;

	}

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (unsigned int i = 0; i < GFX::listeners.len; i++)
		((void (*)()) GFX::listeners.get(i))();

	WIN::swapBuffers();
	GFX::frame++;

}
