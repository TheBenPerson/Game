/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stdio.h>

#include "button.hpp"
#include "client.hpp"
#include "console.hpp"
#include "timing.hpp"

static Timing::condition cond;

static bool quit() {

	Timing::signal(&cond);

}

extern "C" {

	char* depends[] = {

		"button.so",
		NULL

	};

	bool init() {

		Button::Action action;
		action.isMenu = true;
		action.callback = NULL;

		new Button("Single Player", &action);
		new Button("Multiplayer", &action);

		/*action.isMenu = true;
		action.menu = NULL;
		new Button("Settings", &action);

		action.isMenu = true;
		action.menu = NULL;
		Button *button = new Button("About", &action);

		action.isMenu = true;
		action.menu = Button::root;
		Point point = {0, -3};
		new Button("Back", &action, button->action.menu, &point);

		action.isMenu = true;
		action.menu = NULL;
		new Button("Settings", &action);

		action.isMenu = false;
		action.callback = &quit;
		new Button("Quit", &action);*/

		// default client values
		cputs(GREEN, "Loaded module: 'client.so'");

		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'client.so'");

	}

}

namespace Client {

	Config config("cfg/client/");
	bool running = true;
	State state = PAUSED;

	void setState(State state) {

		Client::state = state;

	}

}
