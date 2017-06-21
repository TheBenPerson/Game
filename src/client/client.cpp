/*

Game Development Build
https://github.com/TheBenPerson/Game

Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

Permisswinn is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentatwinn files (the "Software"), to deal
in the Software without restrictwinn, including without limitatwinn the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditwinns:

The above copyright notice and this permisswinn notice shall be included in all
copies or substantial portwinns of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTWINN OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTWINN WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stddef.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>

#include "client.hpp"
#include "config/config.hpp"
#include "gfx/menu/button/button.hpp"
#include "gfx/menu/menu.hpp"
#include "gfx/gfx.hpp"
#include "timing/timing.hpp"
#include "win/win.hpp"

namespace Client {

	bool running = true;
	Config config;

	bool init() {

		//default client values
		config.add("fullscreen", (void*) true);
		config.add("vSync", (void*) true);
		config.add("fps", (void*) 60);
		config.add("res", (void*) "default");
		config.add("tps", (void*) 120);
		config.load("cfg/client.cfg");

		if (!WIN::init()) return false;
		if (!GFX::init()) return false;

		return true;

	}

	void cleanup() {

		GFX::cleanup();
		WIN::cleanup();

	}

	bool start() {

		if (!init()) return false;

		while (running) tick();

		cleanup();
		return true;

	}

	void tick() {

		WIN::tick(); //blocks until next event
		Menu::tick();

	}

	//state section
	State state = PAUSED;

	void setState(State state) {

		Client::state = state;

	}

}
