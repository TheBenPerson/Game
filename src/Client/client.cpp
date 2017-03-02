/*

Copyright (C) 2016-2017 Ben Stockett

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

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "client.hpp"
#include "Rendering/Menu/Button/button.hpp"
#include "Rendering/Menu/menu.hpp"
#include "Rendering/rendering.hpp"
#include "Util/Config/config.hpp"
#include "Util/Interval/interval.hpp"
#include "XClient/xclient.hpp"
#include "XInput/xinput.hpp"

Config Client::config;
bool Client::running = true;
pthread_t Client::thread;

void Client::cleanup() {

	running = false;
	pthread_join(thread, NULL);

	XInput::cleanup();
	Rendering::cleanup();

}

bool Client::init() {

	config.add("fullscreen", (void*) true);
	config.add("vSync", (void*) true);
	config.add("fps", (void*) 60);
	config.add("tps", (void*) 120);
	config.load("etc/client.cfg");

	if (!Rendering::init())
		return false;

	XInput::init();

	pthread_attr_t attrib;
	pthread_attr_init(&attrib);

	pthread_create(&thread, &attrib, threadMain, NULL);

	pthread_attr_destroy(&attrib);

	return true;

}

bool Client::start() {

	if (!init())
		return false;

	doInterval(&XClient::tick, (time_t) 120, false, &running);

	cleanup();

	return true;

}

void* Client::threadMain(void*) {

	doInterval(&tick, (time_t) config.get("tps")->val, false, &running);
	return NULL;

}

void Client::tick() {

	for (size_t i = 0; i < Menu::menuLen; i++)
		Menu::panel[i].tick();

}
