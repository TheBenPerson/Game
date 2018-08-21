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
#include <math.h>
#include <png.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.hh"
#include "gfx.hh"
#include "point.hh"
#include "string.hh"
#include "timing.hh"
#include "win.hh"

namespace GFX {

	NodeList listeners;
	unsigned int frame;

}

static Config *config;

static Timing::Condition condition;
static Timing::thread t;
static Timing::mutex m = MTX_DEFAULT;
static uintptr_t (*callback)(uintptr_t) = NULL;
static uintptr_t arg;
static bool running = true;
static GLuint font;

static void* threadMain(void*);
static void initOGL();
static GFX::texture loadTexture(char *name);
static void freeTexture(GFX::texture *tex);
static void draw();

extern "C" {

	bool init() {

		Config::Option options[] = {

			INT("fps", 60),
			STRING("res", "default"),
			END

		};

		config = new Config("cfg/client/gfx.cfg", options);

		struct {

			Timing::Condition condition;
			bool result = true;

		} args;

		// thread returns result in result
		t = Timing::createThread(threadMain, (void*) &args);

		Timing::waitFor(&args.condition);

		if (!args.result) {

			Timing::waitFor(t);
			return false;

		}

		return true;

	}

	void cleanup() {

		running = false;
		Timing::waitFor(t);

	}

}

// global functions

namespace GFX {

	uintptr_t call(uintptr_t (*function)(uintptr_t), uintptr_t arg) {

		// limit call to one thread at a time
		Timing::lock(&m);

		callback = function;
		::arg = arg;

		Timing::waitFor(&condition);
		Timing::unlock(&m);

		return ::arg;

	}

	void drawText(char *text, Point *point, float size, bool center, float rot) {

		Point dimTex = {16, 6};

		Point dim = {1 / 16.0f, 1 / 6.0f};
		dim *= size * 6;

		float spacing = 0;

		size_t len = strlen(text);

		Point pos = *point;
		float dx = 0;

		if (center) {

			char *start = text;
			char *end;

			unsigned int lines = 0;
			size_t best = 0;

			// get number of lines and max dimChar.x
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

			dx = ((dim.x + spacing) * best) / 2;

			pos.x -= dx;
			pos.y -= ((lines * dim.y) / 2) - dim.y;

		}

		if (rot) {

			// opengl uses degrees ¯\_(ツ)_/¯
			float deg = (rot * 360) / (M_PI * 2);

			glPushMatrix();
			glTranslatef(point->x, point->y, 0);
			glRotatef(deg, 0, 0, 1);
			glTranslatef(-point->x, -point->y, 0);

		}

		for (unsigned int i = 0; i < len; i++) {

			if (text[i] == '\n') {

				pos.y -= dim.y;
				pos.x = point->x - dx;
				continue;

			} else if (text[i] == '\t') {

				pos.x += dim.x * 4;
				continue;

			}

			Point tpos = pos;
			tpos.x += dim.x / 2;
			tpos.y -= dim.y / 2;

			Point frame;
			frame.x = (text[i] - 32) % (int) dimTex.x;
			frame.y = dimTex.y - ((text[i] - 32) / (int) dimTex.x) - 1;

			drawSprite(font, &tpos, &dim, NULL, &dimTex, &frame);

			pos.x += dim.x + spacing;

		}

		if (rot) glPopMatrix();

	}

	void drawSprite(GLuint tex, Point *pos, Point *dim, float rot, Point *tdim, Point *frame) {

		glPushMatrix();

		glTranslatef(pos->x, pos->y, 0);

		if (rot) {

			// opengl uses degrees ¯\_(ツ)_/¯
			float deg = (rot * 360) / (M_PI * 2);
			glRotatef(deg, 0, 0, 1);

		}

		glScalef(dim->x / 2, dim->y / 2, 1);

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex);

		if (tdim) {

			// size of a single frame in texture coords
			float width = 1 / tdim->x;
			float height = 1 / tdim->y;

			float xoff = width * frame->x;
			float yoff = height * frame->y;

			glTranslatef(xoff, yoff, 0);
			glScalef(width, height, 1);

		}

		glBegin(GL_QUADS);

		glTexCoord2f(0, 1);
		glVertex2f(-1, 1);

		glTexCoord2f(0, 0);
		glVertex2f(-1, -1);

		glTexCoord2f(1, 0);
		glVertex2f(1, -1);

		glTexCoord2f(1, 1);
		glVertex2f(1, 1);

		glEnd();

		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	}

	texture loadTexture(char *name) {

		// cryptic, right?
		return call((uintptr_t (*)(uintptr_t)) &::loadTexture, (uintptr_t) name);

	}

	void freeTexture(texture *tex) {

		call((uintptr_t (*)(uintptr_t)) &::freeTexture, (uintptr_t) tex);

	}

}

// static functions

void* threadMain(void* result) {

	struct Args {

		Timing::Condition condition;
		bool result;

	} *args = (Args*) result;

	if (!WIN::initContext()) {

		 // tell main thread init failed
		args->result = false;
		Timing::signal(&args->condition);

		return NULL;

	}

	font = loadTexture("font.png");
	initOGL(); // initialize OpenGL

	WIN::showWindow(); // display window
	Timing::signal(&args->condition);

	if (WIN::vSync) while (running) draw();
	else Timing::doInterval(&draw, (time_t) config->getInt("fps"), false, &running);

	glDeleteTextures(1, &font);
	WIN::cleanupContext();

	return NULL;

}


void initOGL() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

GFX::texture loadTexture(char *name) { // consider strlen instead of constants

	char *res = config->getStr("res");
	STRING_CAT4(buf, "res/texture/", res, "/", name);

	FILE *file = fopen(buf, "r");
	if (!file) {

		STRING_CAT2(buf, "res/texture/default/", name);

		file = fopen(buf, "r");
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
		callback = NULL;

		// tell caller to return
		Timing::signal(&condition);

	}

	if (WIN::resized) {

		glViewport(0, 0, WIN::width, WIN::height);
		double aspect = (double) WIN::width / (double) WIN::height;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-10 * aspect, 10 * aspect, -10, 10, -10, 10);

		WIN::resized = false;

	}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (unsigned int i = 0; i < GFX::listeners.size; i++)
		((void (*)()) GFX::listeners[i])();

	WIN::swapBuffers();
	GFX::frame++;

}
