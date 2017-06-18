/*

Game Development Build
https://github.com/TheBenPerson/Game

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

#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/data.cpp"

void sigHandler(int signal);

int main(int argc, char* const argv[]) {

	struct sigaction action;
	memset(&action, NULL, sizeof(struct sigaction));
	action.sa_handler = &sigHandler;

	sigaction(SIGINT, &action, NULL); //install signal handlers
	sigaction(SIGSEGV, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGHUP, &action, NULL);

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ); //make stdout and stderr line buffered
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ); //this ensures output is instant

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

			case 'h':

				puts(usage);
				return 0;

			break;
			case 'v':

				puts(Data::versionString);
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
			case '?':

				puts(usage);
				return 1;

			break;

		}

	}

	void* handle;

	if (isServer) {

		handle = dlopen("libserver.so", RTLD_LAZY);
		if (!handle) {

			fprintf(stderr, "Error loading %s\n", dlerror());
			return 1;

		}

		printf("Loaded libserver.so\nStarting server on port %i...\n", port);

		bool (*serverStart)(bool, uint16_t) = (bool (*)(bool, uint16_t)) dlsym(handle, "_ZN6Server5StartEt");
		return serverStart(false, port) ? 0 : 1;

	} else {

		handle = dlopen("libclient.so", RTLD_LAZY);
		if (!handle) {

			fprintf(stderr, "Error loading %s\n", dlerror());
			return 1;

		}

		puts("Loaded libclient.so\nStarting Client...\n");

		bool (*clientStart)() = (bool (*)()) dlsym(handle, "_ZN6Client5startEv");
		return !clientStart();

	}

}

void sigHandler(int signal) {

	fputs("\nRecieved ", stdout);

	switch (signal) {

		case SIGINT:

			fputs("SIGINT", stdout);

		break;

		case SIGSEGV:

			fputs("SIGSEGV", stdout);

		break;

		case SIGTERM:

			fputs("SIGINT", stdout);

		break;

		case SIGHUP:

			fputs("SIGHUP", stdout);

		break;

	}

	puts(" - exiting...");
	exit(signal);

}
