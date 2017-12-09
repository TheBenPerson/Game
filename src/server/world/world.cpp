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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.hpp"
#include "world.hpp"

extern "C" {

	char* depends[] = {

		// doesn't requre net.so: won't try anything if no clients
		"server.so",
		NULL

	};

	bool init() {

		bool result = World::loadMap("res/map/main.map");
		if (!result) return false;

		cputs(GREEN, "Loaded module: 'world.so'");
		return true;

	}

	void cleanup() {

		free(World::data);
		cputs(YELLOW, "Unloaded module: 'world.so'");

	}

}

namespace World {

	Tile *data = NULL; // should be mutex locked...
	unsigned int width;
	unsigned int height;

	bool loadMap(char path[]) {

		FILE *file = fopen(path, "r");
		if (!file) {

			ceprintf(RED, "Error opening file '%s': %s\n", path, strerror(errno));
			return false;

		}

		char buf[4];
		buf[0] = '\0';
		buf[1] = '\0';
		buf[2] = '\0';
		buf[3] = '\0';

		char c = fgetc(file);
		if ((c >= '0') && (c <= '9')) {

			buf[0] = c;

			c = fgetc(file);
			if ((c >= '0') && (c <= '9')) {

				buf[1] = c;

				c = fgetc(file);
				if ((c >= '0') && (c <= '9')) buf[2] = c;

			}

			width = atoi(buf);

		} else {

			fclose(file);

			ceprintf(RED, "Error parsing map '%s'\n", path);
			return false;

		}

		buf[0] = '\0';
		buf[1] = '\0';
		buf[2] = '\0';

		c = fgetc(file);
		if ((c >= '0') && (c <= '9')) {

			buf[0] = c;

			c = fgetc(file);
			if ((c >= '0') && (c <= '9')) {

				buf[1] = c;

				c = fgetc(file);
				if ((c >= '0') && (c <= '9')) buf[2] = c;

			}

			height = atoi(buf);

		} else {

			fclose(file);

			ceprintf(RED, "Error parsing map '%s'\n", path);
			return false;

		}

		size_t size = width * height;

		if (data) free(data);
		data = (Tile*) malloc(sizeof(Tile) * size);

		for (size_t i = 0; i < size; i++) {

			for (;;) {

				char buf[3];
				buf[1] = '\0';
				buf[2] = '\0';

				char c = fgetc(file);
				if ((c >= '0') && (c <= '9')) {

					buf[0] = c;

					c = fgetc(file);
					if ((c >= 0x30) && (c <= 0x39)) buf[1] = c;

					data[i].val = atoi(buf);
					break;

				}

			}

		}

		printf("Loaded map '%s' (%ix%i)\n", path, width, height);
		return true;

	}

}
