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

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
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
	void (*cleanup)();

} Module;

static char *base;
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
		"-h, --help\t\t"			"display this message and exit\n\t"
		"-v, --version\t\t"			"display version info and exit\n\t"
		"-s, --server <port>\t" 	"start the server on the specified port";

	bool isServer = false;

	char c;
	option options[] = {

		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"server", required_argument, NULL, 's'},
		{NULL, NULL, NULL, NULL}

	};

	int index;

	for (;;) {

		c = getopt_long(argc, argv, "hvs:", options, &index);
		if (c == -1) break;

		switch (c) {

			case 'h': puts(usage);
			return 0;

			case 'v': puts(Data::versionString);
			return 0;

			case 's':

				char* flag;
				Game::port = (uint16_t) strtol(optarg, &flag, 10);

				if (*flag != '\0') {

					fprintf(stderr, "%s: invalid port value -- '%s'\n", argv[0], optarg);
					puts(usage);

					return 1;

				} else isServer = true;

			break;
			case '?': puts(usage);
			return 1;

		}

	}

	char *path = (char*) malloc(20);
	sprintf(path, "cfg/%s/mods.txt", isServer ? "server" : "client");

	FILE *file = fopen(path, "r");
	if (!file) {

		ceprintf(RED, "Error opening '%s': %s -- exiting...\n", path, strerror(errno));

		free(path);
		return 1;

	}

	free(path);
	base = isServer ? "bin/server/" : "bin/client/";

	bool result = false;

	for (;;) {

		char *name = NULL;
		size_t n = NULL;

		if (getline(&name, &n, file) == -1) {

			free(name);
			break;

		}

		if (name[strlen(name) - 1] == '\n') name[strlen(name) - 1] = '\0';

		result = Game::loadModule(name);
		free(name);

		if (!result) break;

	}

	fclose(file);
	if (result) Timing::waitFor(&cond);

	for (int i = modules.len - 1; i >= 0; i--) {

		Module *module = (Module *) modules.get(i);
		if (module->cleanup) module->cleanup();

		delete module;

	}

	return 0;

}

namespace Game {

	uint16_t port;

	bool loadModule(char *path) {

		char *name = (char*) malloc(strlen(base) + strlen(path) + 1);
		sprintf(name, "%s%s", base, path);

		void *handle = dlopen(name, RTLD_LAZY);
		free(name);

		if (!handle) {

			ceprintf(RED, "Error loading module: '%s'\n", dlerror());
			return false;

		}

		// has the module been loaded already?
		for (unsigned int i = 0; i < modules.len; i++) {

			if (((Module*) modules.get(i))->handle == handle) {

				dlclose(handle);
				return true;

			}

		}

		Module *mod = new Module();
		mod->handle = handle;

		char **depends = (char**) dlsym(handle, "depends");
		if (depends) {

			// load module dependancies
			for (; *depends; depends++) {

				if (!loadModule(*depends)) {

					delete mod;

					ceprintf(RED, "Error loading module '%s': error loading dependency '%s'\n", path, *depends);
					dlclose(handle);

					return false;

				}

			}

		}

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
		modules.add((void*) mod);

		return true;

	}

	void stop() {

		Timing::signal(&cond);

	}

}
