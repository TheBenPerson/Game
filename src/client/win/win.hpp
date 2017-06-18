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

#ifndef GAME_CLIENT_WIN

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

#include "point/point.hpp"

namespace WIN {

	bool init();
	void cleanup();

	//screen details
	extern bool fullscreen;
	extern unsigned int screenHeight;
	extern unsigned int screenWidth;
	extern float aspect;
	extern bool vSync;

	void setFullscreen(bool mode);

	//window details
	extern unsigned int height;
	extern unsigned int width;

	bool createWindow();
	bool initContext();
	void cleanupContext();
	void tick();

	//input details
	typedef struct {

		xcb_keycode_t key;
		bool state;
		void (*callback)();

	} Key;

	enum Action { A_UP, A_DOWN, A_LEFT, A_RIGHT, A_EXIT, A_ACTION, A_FULLSCREEN, A_NUM_ACTIONS };

	extern Key keys[];
	extern Point mouse;

	void initInput();
	void loadKeys();
	void actnUp();
	void actnDown();
	void actnLeft();
	void actnRight();
	void actnExit();
	void actnAction();
	void actnFullscreen();

	//OS details
	extern xcb_connection_t *connection;
	extern GLXContext context;
	extern Display* display;
	extern xcb_window_t rootWin;
	extern xcb_window_t winID;
	extern xcb_atom_t NET_WM_STATE;
	extern xcb_atom_t NET_WM_STATE_FULLSCREEN;
	extern xcb_atom_t WM_DELETE_WINDOW;

	bool initX11();

}

#define GAME_CLIENT_WIN
#endif
