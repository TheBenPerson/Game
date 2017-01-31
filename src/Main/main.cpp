#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Client/Client/client.hpp"
#include "Client/Networking/networking.hpp"
#include "Server/Server/server.hpp"

int main(int argc, const char ** argv) {

	bool Error = false;
	const char * ErrorMsg = NULL;
	bool Help = false;
	const char * HelpMsg = "Usage: Game [Options]\n\nOptions:\n\t-h, --help\t\tDisplay this message.\n\t-s, --server <port>\tStart the server on the specified port.\n";
	bool IsDedicatedServer = false;
	unsigned short int Port;

	for (int i = 1; i < argc; i++) {

		if ((strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--server") == 0)) {

			IsDedicatedServer = true;

			if (++i == argc) {

				Error = true;
				break;

			} else  {

				Port = atoi(argv[i]);

				if (Port < 1) {

					Error = true;
					ErrorMsg = argv[i];

					break;

				}

			}

		} else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {

			Help = true;

		} else {

			Error = true;
			ErrorMsg = argv[i];

			break;

		}

	}

	if (Error) {

		if (IsDedicatedServer) {

			if (ErrorMsg) {

				printf("Game: Invalid port number -- '%s'\n%s", ErrorMsg, HelpMsg);

			} else {

				printf("Game: No port number given.\n%s", HelpMsg);

			}

		} else {

			printf("Game: Invalid option -- '%s'\n%s", ErrorMsg, HelpMsg);

		}

		return 0;

	}

	if (Help) {

		printf(HelpMsg);
		return 0;

	}

	if (IsDedicatedServer) {

		printf("Starting server on port %i...\n", Port);
		return Server::Start(false, Port) ? 0 : 1;

	} else {

		printf("Starting client...\n");
		return Client::Start() ? 0 : 1;

	}

}
