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
