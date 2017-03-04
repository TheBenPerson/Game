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

#ifndef HG_Client_Rendering_Window_H

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

namespace XClient {

	extern float aspect;
	extern xcb_connection_t *connection;
	extern GLXContext context;
	extern bool fullscreen;
	extern unsigned int height;
	extern Display* display;
	extern xcb_window_t rootWin;
	extern unsigned int screenHeight;
	extern unsigned int screenWidth;
	extern bool vSync;
	extern unsigned int width;
	extern xcb_window_t winID;
	extern xcb_atom_t NET_WM_STATE;
	extern xcb_atom_t NET_WM_STATE_FULLSCREEN;
	extern xcb_atom_t WM_DELETE_WINDOW;

	void cleanup();
	bool createWindow();
	bool finalizeContext();
	void setFullscreen(bool mode);
	void tick();

}

#define HG_Client_Rendering_Window_H
#endif
