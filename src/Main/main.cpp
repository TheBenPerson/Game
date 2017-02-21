#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util/Config/config.hpp"
#include "Util/NodeList/nodelist.hpp"

void crashHandler(int);

int main(int argc, char* const argv[]) {

	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = &crashHandler;

	sigaction(SIGHUP, &action, NULL);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGSEGV, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);

	const char* usage = "Usage: game [Options]\n\nOptions:\n\t-h, --help\t\tDisplay this message.\n\t-s, --server <port>\tStart the server on the specified port.";
	bool isServer = false;
	uint16_t port;

	char c;
	option options[] = {

		{"help", no_argument, NULL, 'h'},
		{"server", required_argument, NULL, 's'},
		{NULL, NULL, NULL, NULL}

	};

	int index;

	for (;;) {

		c = getopt_long(argc, argv, "hs:", options, &index);
		if (c == -1) break;

		switch (c) {

			case 'h':

				puts(usage);
				return 0;

			break;
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

void crashHandler(int signal) {

	switch (signal) {

		case SIGHUP:

			puts("\nRecieved SIGHUP");

		break;

		case SIGINT:

			puts("\nRecieved SIGINT");

		break;

		case SIGSEGV:

			puts("Recieved SIGSEV");

		break;

		case SIGTERM:

			puts("Recieved SIGTERM");

		break;

		case SIGQUIT:

			puts("Recieved SIGQUIT");

		break;

	}

	exit(signal);

}
