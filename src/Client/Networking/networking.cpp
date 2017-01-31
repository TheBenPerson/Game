#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "networking.hpp"
#include "Util/Packet/packet.hpp"

bool Networking::running = false;
int Networking::socket;
pthread_t Networking::thread;

void Networking::cleanup() {

	close(socket);

}

void* Networking::packetHandler(void*) {

	Packet tPacket;

	while (running) {

		socklen_t len = sizeof(sockaddr_in);

		int ready = recv(socket, tPacket.raw, P_MAX_SIZE, 0);

		if (ready != -1) {

			if (tPacket.id & 0x80) {

				switch (tPacket.id & 0x7F) {

					case P_ACT:

						printf("Connected to server.\n");

				}

			} else {

				switch (tPacket.id & 0x7F) {



				}

			}

		} else {

			usleep(10000);

		}

	}

}

bool Networking::init(uint16_t port) {

	socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (socket == -1) {

		fprintf(stderr, "Error creating socket: %s.\n", strerror(errno));
		return false;

	}

	fcntl(socket, F_SETFL, fcntl(socket, F_GETFL) | O_NONBLOCK);

	return true;

}

void Networking::start(char* address) {

	running = true;

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&thread, &attr, packetHandler, NULL);

	pthread_attr_destroy(&attr);

}

void Networking::stop() {

	running = false;
	pthread_join(thread, NULL);

	//Disconnect from server if necesary

}
