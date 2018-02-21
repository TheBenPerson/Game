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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.hh"

Config::Config(char *dir): dir(dir) {}

void Config::load(char *base) {

	char *path = (char*) malloc(strlen(dir) + strlen(base) + 1);
	sprintf(path, "%s%s", dir, base);

	FILE *file = fopen(path, "r");
	free(path);

	if (!file) {

		fprintf(stderr, "Error loading config '%s': %s\n", base, strerror(errno));
		return;

	}

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

				fclose(file);

				fprintf(stderr, "Error loading config '%s': invalid format\n", base);

				free(buf);
				return;

			}

			val = strtok(NULL, "=:\t ");
			if (!val) {

				fclose(file);

				fprintf(stderr, "Error loading config '%s': invalid format\n", base);

				free(buf);
				return;

			}

			bool isString = false;

			char* flag;
			void *nVal = (void*) strtol(val, &flag, 10); // TODO: rename nVal to something else

			if (*flag) {

				if (!strcasecmp(val, "true")) nVal = (void*) true;
				else if (!strcasecmp(val, "false")) nVal = (void*) false;
				else isString = true;

			}

			if (isString) nVal = strdup(val);

			// key format is file.key
			// where file is the file name without any suffix (.conf, .cfg)
			// and key is just the key in the config file

			char *end = strchr(base, '.');
			if (!end) end = base + strlen(base); // TODO: strchrnul if compatible
			unsigned int len = end - base;

			char *buf = new char[len + strlen(name) + 2];

			strncpy(buf, base, len);
			buf[len] = '.';
			strcpy(buf + len + 1, name);

			set(buf, nVal, isString);

			delete[] buf;

		}

		free(buf);
		if (result == -1) break;

	}

	fclose(file);
	printf("Loaded config: '%s'\n", base);

}

// TODO: saving things

/*static void addwrite(FILE *file, ) {

	free(key->name);

	switch (key->t) {

		case hval: free(key->val);
		case val: delete key;
		return;

		case type::key:
		break;

	}

	NodeList *list = (NodeList*) key->val;
	delete key;

	for (unsigned int i = 0; i < list->len; i++) {

		key = (Key*) list->get(i);
		del(key);

	}

	delete list;

}

void Config::save(char *path) {

	for (unsigned int i = 0; i < list->len; i++) {

		key = (Key*) list->get(i);
		addwrite(key);

	}

}*/