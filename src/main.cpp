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

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.hpp"
#include "main.hpp"

NodeList modules;

bool loadMods(char *path);
void loadMod(char *base, char *path);
void sigHandler(int signal);

int main(int argc, char* argv[]) {

	signal(SIGSEGV, &sigHandler);
	signal(SIGHUP, &sigHandler);

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ); // make stdout and stderr line buffered
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ); // this ensures output is instant

	const char* usage =
	"Usage: game [Options]\n\n"

	"Options:\n\t"
		"-h, --help\t\t"			"display this message and exit\n\t"
		"-v, --version\t\t"			"display version info and exit\n\t"
		"-s, --server <port>\t" 	"dtart the server on the specified port";

	bool isServer = false;
	uint16_t port;

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
				port = (uint16_t) strtol(optarg, &flag, 10);

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

	if (isServer) loadMods("bin/server/");
	else loadMods("bin/client/");

	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);
	sigaddset(&sigset, SIGTERM);

	sigprocmask(SIG_BLOCK, &sigset, NULL);

	int signal;
	sigwait(&sigset, &signal);

	printf("%s - Cleaning up...\n", strsignal(signal));

	for (int i = modules.len - 1; i >= 0; i--) {

		Module *module = (Module *) modules.get(i);
		if (module->cleanup) module->cleanup();

		delete module;

	}

	return 0;

}

void loadMod(char *base, char *path) {

	char *name = new char[strlen(base) + strlen(path)];
	strcpy(name, base);
	strcpy(name + strlen(base), path);

	void *handle = dlopen(name, RTLD_LAZY);
	delete[] name;

	// has the mod been loaded already?
	for (unsigned int i = 0; i < modules.len; i++)
		if (((Module*) modules.get(i))->handle == handle) return;

	if (handle) {

		Module *mod = new Module();
		mod->handle = handle;

		char **depends = (char**) dlsym(handle, "depends");
		if (depends) {

			// load module dependancies
			for (; *depends; depends++)
				loadMod(base, *depends);

		}

		mod->cleanup = (void (*)()) dlsym(handle, "cleanup");
		bool (*init)() = (bool (*)()) dlsym(handle, "init");

		bool result = true;
		if (init) result = init();
		// mod prints its own message

		if (result) modules.add((void*) mod);
		else {

			delete mod;
			dlclose(handle);

		}

	} else fprintf(stderr, "Error loading module %s\n", dlerror());

}

bool loadMods(char *path) {

	DIR *dir = opendir(path);
	if (!dir) {

		perror("Error loading modules");
		return false;

	}

	for (;;) {

		dirent *file = readdir(dir);
		if (!file) break;

		if (file->d_type != DT_REG) continue;

		loadMod(path, file->d_name);

	}

	closedir(dir);
	return true;

}

void sigHandler(int signal) {

	// I know it's not reentrant, but it's worked so far.
	printf("%s - exiting...\n", strsignal(signal));
	exit(signal);

}
