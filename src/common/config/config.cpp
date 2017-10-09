/*

Game Development Build
https:// github.com/TheBenPerson/Game

Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.hpp"

void Config::add(char *name, void *val) {

	Setting* setting = new Setting();
	setting->name = name;
	setting->val = val;

	setting->freeName = false;
	setting->freeVal = false;

	settings.add(setting);

}

Config::Setting* Config::get(char* name) {

	for (unsigned int i = 0; i < settings.len; i++) {

		Setting *setting = (Setting*) settings.get(i);
		if (!strcmp(setting->name, name)) return setting;

	}

	return NULL;

}

Config::Setting* Config::get(unsigned int i) {

	return (Setting*) settings.get(i);

}

bool Config::load(char *path) {

	FILE *file = fopen(path, "r");
	if (!file) return false;

	for (;;) {

		char *buf = NULL; // auto allocate
		char *name;
		char *val;

		size_t len = NULL; // auto allocate
		ssize_t result = getline(&buf, &len, file);

		// skip line if last, commented, or empty
		if (result != -1 && buf[0] != '#' && buf[0] != '\n') {

			// strip tailing newline
			if (buf[result - 1] == '\n') buf[result - 1] = '\0';

			name = strtok(buf, "=:\t ");
			if (!name) {

				free(buf);
				fclose(file);
				return false;

			}

			name = strdup(name);

			val = strtok(NULL, "=:\t ");
			if (!val) {

				free(name);
				free(buf);
				fclose(file);
				return false;

			}

			val = strdup(val);

			bool isNew;

			Setting *setting = get(name);
			if (setting) {

				free(name);
				if (setting->freeVal) free(setting->val);

				isNew = false;

			} else {

				setting = new Setting();
				setting->name = name;
				setting->freeName = true;

				isNew = true;

			}

			setting->isString = false;

			char* flag;
			long int nVal = strtol(val, &flag, 10);

			if (*flag) {

				if (!strcasecmp(val, "true")) nVal = true;
				else if (!strcasecmp(val, "false")) nVal = false;
				else {

					setting->isString = true;

				}

			}

			if (setting->isString) {

				setting->val = val;
				setting->freeVal = true;

			} else {

				setting->val = (void*) nVal;
				free(val);

				setting->freeVal = false;

			}

			if (isNew) settings.add(setting);

		}

		free(buf);
		if (result == -1) break;

	}

	fclose(file);

	printf("Loaded config: '%s'\n", basename(path));
	return true;

}

void Config::save(char *path) {



}

void Config::set(char *name, void *val) {

	get(name)->val = val;

}

Config::~Config() {

	Setting* setting;

	for (unsigned int i = 0; i < settings.len; i++) {

		setting = (Setting*) settings.get(i);
		if (setting->freeName) free(setting->name);
		if (setting->freeVal) free(setting->val);

		delete setting;

	}

}
