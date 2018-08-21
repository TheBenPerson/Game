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

#include <confuse.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.hh"

Config::Config(char *path, Option *options, bool *success): path(path) {

	cfg = cfg_init(options, NULL);

	int result = cfg_parse(cfg, path);
	if (result != CFG_SUCCESS) {

		if (result == CFG_FILE_ERROR) fprintf(stderr, "Error loading config '%s': %s\n", path, strerror(errno));
		else cfg_error(cfg, "Error loading config '%s'", path);

		if (success) *success = false;
		return;

	}

	printf("Loaded config: '%s'\n", path);
	if (success) *success = true;

}

Config::~Config() {

	/*FILE *file = fopen(path, "w");
	if (file) {

		cfg_print(cfg, file);
		fclose(file);

	} else perror("Error saving file");*/

	// save file
	cfg_free(cfg);

}

bool Config::getBool(char *item, unsigned int index) {

	return cfg_getnbool(cfg, item, index);

}

int Config::getInt(char *item, unsigned int index) {

	return cfg_getnint(cfg, item, index);

}

float Config::getFloat(char *item, unsigned int index) {

	return cfg_getnfloat(cfg, item, index);

}

char* Config::getStr(char *item, unsigned int index) {

	return cfg_getnstr(cfg, item, index);

}

unsigned int Config::getSize(char *item) {

	return cfg_size(cfg, item);

}

void Config::setBool(char *item, bool value) {

	cfg_setbool(cfg, item, (cfg_bool_t) value);

}

void Config::setInt(char *item, int value) {

	cfg_setint(cfg, item, value);

}

void Config::setStr(char *item, char *value) {

	cfg_setstr(cfg, item, value);

}
