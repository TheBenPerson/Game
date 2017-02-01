#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Client/client.hpp"
#include "Client/Networking/networking.hpp"
#include "Server/server.hpp"

int main(int argc, const char** argv) {

	bool error = false;
	const char * errorMsg = NULL;
	bool help = false;
	const char * helpMsg = "Usage: Game [Options]\n\nOptions:\n\t-h, --help\t\tDisplay this message.\n\t-s, --server <port>\tStart the server on the specified port.\n";
	bool isDedicatedServer = false;
	unsigned short int port;

	for (int i = 1; i < argc; i++) {

		if ((strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--server") == 0)) {

			isDedicatedServer = true;

			if (++i == argc) {

				error = true;
				break;

			} else  {

				port = atoi(argv[i]);

				if (port < 1) {

					error = true;
					errorMsg = argv[i];

					break;

				}

			}

		} else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {

			help = true;

		} else {

			error = true;
			errorMsg = argv[i];

			break;

		}

	}

	if (error) {

		if (isDedicatedServer) {

			if (errorMsg) {

				printf("Game: Invalid port number -- '%s'\n%s", errorMsg, helpMsg);

			} else {

				printf("Game: No port number given.\n%s", helpMsg);

			}

		} else {

			printf("Game: Invalid option -- '%s'\n%s", errorMsg, helpMsg);

		}

		return 0;

	}

	if (help) {

		printf(helpMsg);
		return 0;

	}

	if (isDedicatedServer) {

		printf("Starting server on port %i...\n", port);
		//return Server::start(false, port) ? 0 : 1;

	} else {

		printf("Starting client...\n");
		return Client::start() ? 0 : 1;

	}

}
