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

#include "button.hh"
#include "console.hh"
#include "win.hh"

Button::Menu *Button::root;

static void del(Button::Menu *menu) {

	for (unsigned int m = 0; m < 2; m++) {

		for (unsigned int i = 0; i < menu->lists[m].len; i++) {

			Button *button = (Button*) menu->lists[m].get(i);
			delete button;

		}

	}

	delete menu;

}

extern "C" {

	bool init() {

		Button::root = new Button::Menu;
		Button::root->parent = NULL;

		cputs(GREEN, "Loaded module: 'button.so'");

		return true;

	}

	void cleanup() {

		del(Button::root);
		cputs(YELLOW, "Unloaded module: 'button.so'");

	}

}

Button::Button(char *name, Action *action, Menu *parent, Point *pos): name(name) {

	this->action = *action;

	if (this->action.isMenu && !this->action.menu) {

		this->action.menu = new Menu;
		this->action.menu->parent = parent;

		Action action;
		action.menu = (Menu*) -1;
		action.isMenu = true;

		new Button("Back", &action, this->action.menu);

	}

	if (pos) {

		this->pos = new Point(*pos);
		parent->lists[1].add((void*) this);

	} else {

		this->pos = NULL;

		if (parent == root) parent->lists[0].add((void*) this);
		else parent->lists[0].add((void*) this, parent->lists[0].len - 1);

	}

}

Button::~Button() {

	if (pos) delete pos;
	if (action.isMenu && action.menu != (Menu*) -1) del(action.menu);

}
