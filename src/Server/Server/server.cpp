#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "Networking/Networking.hpp"
#include "server.hpp"

bool Server::local;
bool Server::running = false;
pthread_t Server::thread;

void Server::cleanup() {

	Networking::cleanup();

}

bool Server::init(uint16_t port) {

	if (!Networking::init(port))
		return false;

	//Game init

	Networking::start();

	return true;

}

bool Server::start(bool local, uint16_t port) {

	Server::local = local;

	if (!Init(port))
		return false;

	running = true;

	if (local) {

		pthread_attr_t attr;
		pthread_attr_init(&attr);

		pthread_create(&thread, &attr, tickLoop, NULL);

		pthread_attr_destroy(&attr);

	} else {

		tickLoop(NULL);

	}

	return true;

}

void Server::stop() {

	Networking::stop();

	running = false;

	if (local)
		pthread_join(thread, NULL);

}

void * Server::tickLoop(void *) {

	while (running) {

		//printf("tick\n");
		usleep(10000);

	}

}
