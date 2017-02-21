#include <errno.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <math.h>
#include <png.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "Client/client.hpp"
#include "Menu/Button/button.hpp"
#include "Menu/menu.hpp"
#include "rendering.hpp"
#include "Util/Interval/interval.hpp"
#include "Util/Point/point.hpp"
#include "XClient/xclient.hpp"

GLuint Rendering::font;
bool Rendering::resized = false;
pthread_t Rendering::thread;

void Rendering::cleanup() {

	pthread_join(thread, NULL);

	XClient::cleanup();

}

void Rendering::draw() {

	if (resized) {

		glViewport(0, 0, XClient::width, XClient::height);
		double aspect = (double) XClient::width / (double) XClient::height;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-10 * aspect, 10 * aspect, -10, 10, -10, 10);

		resized = false;

	}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	char message[100];
	sprintf(message, "Width: %i\nHeight: %i", XClient::width, XClient::height);
	drawText(message, {-3.0f, 5.0f}, 10.0f, 1.0f, false);

	drawText("Game\nBuild Version 1.0.0", {((double) XClient::width / (double) XClient::height * -10) + 0.5f, 9.5f}, 10.0f, 1.2f, false);
	drawText("abcdefghijklmnopqrstuvwxyz", {-8, -8}, 10, 1, false);
	drawText("ABCDEFGHIJKLMNOPQRSTUVWXYZ", {-8, -6}, 10, 1, false);

	if (Menu::inGame) {



	} else {



	}

	if (Menu::active) {

		for (unsigned int i = 0; i < Menu::panels[Menu::mode].len; i++)
			((Button *) Menu::panels[Menu::mode].get(i))->render();

	}

	glXSwapBuffers(XClient::display, XClient::winID);

}

void Rendering::glCleanup() {

	Menu::cleanup();

	glDeleteTextures(1, &font);

}

void Rendering::glInit() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	font = loadTexture("res/font.png");

	Menu::init();

}

bool Rendering::init() {

	if (!XClient::createWindow()) return false;

	pthread_attr_t attrib;
	pthread_attr_init(&attrib);

	volatile char result = -1;
	pthread_create(&thread, &attrib, Rendering::threadMain, (void *) &result);

	pthread_attr_destroy(&attrib);

	while (result == -1) {}

	if (!result) {

		pthread_join(thread, NULL);
		return false;

	}

	return true;

}

GLuint Rendering::loadTexture(const char* path) {

	FILE* file = fopen(path, "r");
	if (!file) {

		fprintf(stderr, "Error loading texture %s: %s\n", path, strerror(errno));
		return 0;

	}

	unsigned char sig[8];
	fread(sig, 8, 1, file);

	if (png_sig_cmp(sig, 0, 8)) {

		fprintf(stderr, "Error loading texture %s: Invalid PNG file.\n", path);
		fclose(file);

		return 0;

	}

	png_struct* png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_info* info = png_create_info_struct(png);

	png_init_io(png, file);
	png_set_sig_bytes(png, 8);

	png_read_info(png, info);

	png_uint_32 width = png_get_image_width(png, info);
	png_uint_32 height = png_get_image_height(png, info);

	png_byte* image = (png_byte*) malloc(width * height * 4);

	for (int i = height - 1; i > 0; i--)
		png_read_row(png, image + (i * width * 4), NULL);

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	free(image);
	png_destroy_read_struct(&png, &info, NULL);
	fclose(file);

	printf("Loaded texture %s (%ix%i)\n", path, width, height);
	return texture;

}

void* Rendering::threadMain(void* result) {

	if (!XClient::finalizeContext()) {

		*((char *) result) = 0;
		return NULL;

	}

	glInit();
	xcb_map_window(XClient::connection, XClient::winID);

	*((unsigned char *) result) = 1;

	if (XClient::vSync) while (Client::running) draw();
	else doInterval(&draw, 60, false, &Client::running);

	glCleanup();

	return NULL;

}

void Rendering::drawText(const char* text, Point position, float maxWidth, float size, bool delayed) {

	float charWidth = 0.454545454545f * size;
	float charHeight = 1.0f * size;
	float spaceWidth = 0.1f * size;

	unsigned int strLen = strlen(text);

	float x = position.x;
	float y = position.y - charHeight;

	glBindTexture(GL_TEXTURE_2D, font);

	glBegin(GL_QUADS);

		for (unsigned int i = 0; i < strLen; i++) {

			if (text[i] == '\n') {

				y -= charHeight;
				x = position.x;
				continue;

			}

			if (x + charWidth > maxWidth) {

				x = position.x;
				x -= charHeight;

			}

			float offset = 0.009765625f * (text[i] - 32);

			glTexCoord2f(offset, 1.0f);
			glVertex2f(x, y + charHeight);
			glTexCoord2f(offset + 0.009765625f, 1.0f);
			glVertex2f(x + charWidth, y + charHeight);
			glTexCoord2f(offset + 0.009765625f, 0.3125f);
			glVertex2f(x + charWidth, y);
			glTexCoord2f(offset, 0.3125f);
			glVertex2f(x, y);

			x += charWidth + spaceWidth;

		}

	glEnd();

}
