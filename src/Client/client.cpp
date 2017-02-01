#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "client.hpp"
#include "Rendering/Menu/Button/button.hpp"
#include "Rendering/Menu/menu.hpp"
#include "Rendering/rendering.hpp"
#include "XClient/xclient.hpp"
#include "XInput/xinput.hpp"

bool Client::running = true;
pthread_t Client::thread;

void Client::cleanup() {

	running = false;

	pthread_join(thread, NULL);

	Rendering::cleanup();

}

bool Client::init() {

	if (!Rendering::init())
		return false;

	XInput::init();

	pthread_attr_t attrib;

	pthread_attr_init(&attrib);

	pthread_create(&thread, &attrib, tickLoop, NULL);

	pthread_attr_destroy(&attrib);

	return true;

}

bool Client::start() {

	if (!init())
		return false;

	XClient::messagePump();

	cleanup();

	return true;

}

void Client::tick() {

	for (unsigned int i = 0; i < Menu::panels[Menu::mode].length; i++)
		((Button *) Menu::panels[Menu::mode].get(i))->tick();

}

void* Client::tickLoop(void*) {

	while (running) {

		tick();

		usleep(1000);

	}

	return NULL;

}
