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

#ifndef GAME_CLIENT_GFX_MENU

#include <GL/gl.h>
#include <stddef.h>

#include "button/button.hpp"

namespace Menu {

	extern GLuint bg;
	extern GLuint loading;
	extern size_t len;
	extern Button *panel;
	extern bool aboutt; //delete me plz

	void init();
	void cleanup();
	void tick();
	void draw();
	void actnBack();

	//about
	extern Button about[];

	//main menu
	extern Button main[];
	void actnStart();
	void actnAbout();
	void actnSettings();
	void actnQuit();

	//settings
	extern Button settings[];
	void actnFullscreen();

}

#define GAME_CLIENT_GFX_MENU
#endif
