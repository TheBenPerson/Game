#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "client.hpp"
#include "Rendering/Menu/Button/Button.hpp"
#include "Rendering/Menu/Menu.hpp"
#include "Rendering/rendering.hpp"
#include "XClient/xclient.hpp"
#include "XInput/XInput.hpp"

bool Client::Running = true;
pthread_t Client::Thread;

void Client::Cleanup() {

	Running = false;

	pthread_join(Thread, NULL);

	Rendering::Cleanup();

}

bool Client::Init() {

	if (!Rendering::Init())
		return false;

	XInput::Init();

	pthread_attr_t Attrib;

	pthread_attr_init(&Attrib);

	pthread_create(&Thread, &Attrib, TickLoop, NULL);

	pthread_attr_destroy(&Attrib);

	return true;

}

bool Client::Start() {

	if (!Init())
		return false;

	XClient::MessagePump();

	Cleanup();

	return true;

}

void Client::Tick() {

	for (unsigned int i = 0; i < Menu::Panels[Menu::Mode].Length; i++)
		((Button *) Menu::Panels[Menu::Mode].Get(i))->Tick();

}

void * Client::TickLoop(void *) {

	while (Running) {

		Tick();

		usleep(1000);

	}

	return NULL;

}
