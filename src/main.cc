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

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.hh"
#include "data.hh"
#include "main.hh"
#include "nodelist.hh"
#include "timing.hh"

typedef struct {

	void *handle;

	union {

		void (*cleanup)();
		char *group;

	};

} Module;

static NodeList modules;
static Timing::condition cond;

void sigHandler(int signal) {

	// I know it's not reentrant, but it's worked so far.
	cprintf(WHITE, "%s - exiting...\n", strsignal(signal));
	Game::stop();

}

int main(int argc, char* argv[]) {

	signal(SIGSEGV, &sigHandler);
	signal(SIGHUP, &sigHandler);
	signal(SIGINT, &sigHandler);

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ); // make stdout and stderr line buffered
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ); // this ensures output is instant

	const char* usage =
	"Usage: game [Options]\n\n"

	"Options:\n\t"
		"-h, --help        display this message and exit\n\t"
		"-v, --version     display version info and exit\n\t"
		"-s, --server      start the server\n\t"
		"-p, --port <port> specify the port";

	bool isServer = false;

	int c;
	option options[] = {

		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"server", no_argument, NULL, 's'},
		{"port", required_argument, NULL, 'p'},
		{NULL, NULL, NULL, NULL}

	};

	int index;

	for (;;) {

		c = getopt_long(argc, argv, "hvsp:", options, &index);
		if (c == -1) break;

		switch (c) {

			case 'h': puts(usage);
			return 0;

			case 'v': puts(Data::versionString);
			return 0;

			case 's': isServer = true;
			break;

			case 'p': {

				char* flag;
				Game::port = (uint16_t) strtol(optarg, &flag, 10);

				if (*flag != '\0') {

					fprintf(stderr, "%s: invalid port value -- '%s'\n", argv[0], optarg);
					puts(usage);

					return 1;

				}

			} break;

			case '?': puts(usage);
			return 1;

		}

	}

	bool result = Game::loadModules(isServer, "main.cfg");
	if (result) Timing::waitFor(&cond);

	for (int i = modules.size - 1; i >= 0; i--) {

		Module *module = (Module *) modules[i];
		if (!module->handle) {

			char border[29 + strlen(module->group) + 1];
			memset((void*) border, '=', sizeof(border) - 1);
			border[sizeof(border) - 1] = '\0';

			cputs(BLUE, border);
			cprintf(YELLOW, "Unloading module group: '%s'...\n", module->group);
			cputs(BLUE, border);

			free(module->group);

		} else module->cleanup();

		delete module;

	}

	return 0;

}

namespace Game {

	uint16_t port = NULL;

	bool loadModule(bool isServer, char *path) {

		char *name = (char*) malloc(11 + strlen(path) + 1);
		sprintf(name, "bin/%s/%s", isServer ? "server" : "client", path);

		void *handle = dlopen(name, RTLD_LAZY);
		free(name);

		if (!handle) {

			ceprintf(RED, "Error loading module: '%s'\n", dlerror());
			return false;

		}

		Module *mod = new Module();
		mod->handle = handle;

		bool (*init)() = (bool (*)()) dlsym(handle, "init");

		bool result = true;
		if (init) result = init();
		// mod prints its own message for success and error

		if (!result) {

			delete mod;
			dlclose(handle);

			return false;

		}

		mod->cleanup = (void (*)()) dlsym(handle, "cleanup");
		modules.add((intptr_t) mod);

		return true;

	}

	bool loadModules(bool isServer, char *name) {

		// determine full file name
		unsigned int len = strlen(name);

		char base[7 + len + 1];
		sprintf(base, "%s/%s", isServer ? "server" : "client", name);

		len = strlen(base);

		// todo: unsafe to use sizeof?
		char border[27 + len + 1];
		memset((void*) border, '=', sizeof(border) - 1);
		border[sizeof(border) - 1] = '\0';

		cputs(BLUE, border);
		cprintf(GREEN, "Loading module group: '%s'...\n", base);
		cputs(BLUE, border);

		char path[4 + len + 1];
		sprintf(path, "cfg/%s", base);

		FILE *file = fopen(path, "r");
		if (!file) {

			ceprintf(RED, "Error opening '%s': %s -- exiting...\n", name, strerror(errno));
			return false;

		}

		for (;;) {

			char *name = NULL;
			size_t n = NULL;

			if (getline(&name, &n, file) == -1) {

				free(name);
				break;

			}

			if (name[strlen(name) - 1] == '\n') name[strlen(name) - 1] = '\0';

			bool result = Game::loadModule(isServer, name);
			free(name);

			if (!result) return false;

		}

		fclose(file);

		Module *module = new Module;
		module->handle = NULL;
		module->group = strdup(base);

		modules.add((intptr_t) module);
		return true;

	}

	void stop() {

		Timing::signal(&cond);

	}

}
