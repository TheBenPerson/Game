/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2018 Ben Stockett <thebenstockett@gmail.com>
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

#include "client.hh"
#include "console.hh"
#include "main.hh"
#include "win.hh"

static void start(Button *) {

	// TODO: unload mods on escape key?
	bool result = Game::loadModules("game.cfg");
	if (!result) return;

	Client::state = Client::IN_GAME;
	WIN::setCursor(false);

}

extern "C" {

	bool init() {

		Button::Action action;
		action.isMenu = false;
		action.callback = &start;

		new Button("Single Player", &action);
		new Button("Multiplayer", &action);

		action.isMenu = true;
		action.menu = NULL;
		Client::settings = (new Button("Settings", &action))->action.menu;

		action.isMenu = true;
		action.menu = NULL;
		new Button("About", &action);

		action.isMenu = false;
		action.callback = (void(*) (Button*)) &Game::stop;
		new Button("Quit", &action);

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
	Button::Menu *settings;

	bool running = true;
	State state = PAUSED;

	void setState(State state) {

		Client::state = state;

	}

}
