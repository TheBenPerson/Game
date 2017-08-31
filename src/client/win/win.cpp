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

#include <GL/glx.h>
#include <GL/glxext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

#include "client.hpp"
#include "config/config.hpp"
#include "gfx/gfx.hpp"
#include "win/win.hpp"

namespace WIN {

	bool init() {

		if (!initX11()) return false;
		if (!createWindow()) return false;
		initInput();

		return true;

	}

	void cleanup() {

		glXDestroyContext(display, context);
		xcb_destroy_window(connection, winID);
		XCloseDisplay(display);

	}

	// screen section
	bool fullscreen;
	unsigned int screenHeight;
	unsigned int screenWidth;
	float aspect;
	bool vSync;

	void setFullscreen(bool mode) {

		xcb_client_message_event_t event;

		memset(&event, NULL, sizeof(xcb_configure_notify_event_t));

		event.response_type = XCB_CLIENT_MESSAGE;
		event.format = 32;
		event.window = winID;
		event.type = NET_WM_STATE;
		event.data.data32[0] = mode;
		event.data.data32[1] = NET_WM_STATE_FULLSCREEN;
		event.data.data32[2] = 0;
		event.data.data32[3] = 1;
		event.data.data32[4] = 0;

		xcb_send_event(connection, false, rootWin, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_STRUCTURE_NOTIFY, (const char*) &event);

		fullscreen = mode;

	}

	//window section
	unsigned int height;
	unsigned int width;

	bool createWindow() {

		int attr[] = {

			GLX_RGBA,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_ALPHA_SIZE, 8,
			GLX_DEPTH_SIZE, 24,
			GLX_DOUBLEBUFFER, true,
			NULL

		};

		XVisualInfo *visualInfo = glXChooseVisual(display, 0/*CHANGE THIS!!!*/, attr);

		if (!visualInfo) {

			XCloseDisplay(display);
			fprintf(stderr, "Error choosing visual: no visuals exist that match the required criteria.\n");

			return false;

		}

		uint32_t eventMask =
		XCB_EVENT_MASK_BUTTON_PRESS
		| XCB_EVENT_MASK_BUTTON_RELEASE
		| XCB_EVENT_MASK_KEY_PRESS
		| XCB_EVENT_MASK_KEY_RELEASE
		| XCB_EVENT_MASK_POINTER_MOTION
		| XCB_EVENT_MASK_STRUCTURE_NOTIFY;

		winID = xcb_generate_id(connection);
		xcb_create_window(connection, visualInfo->depth, winID, rootWin, (screenWidth - screenHeight) / 2, 0, screenHeight, screenHeight, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, visualInfo->visualid, XCB_CW_EVENT_MASK, &eventMask);

		char *string;

		string = "Game";
		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, winID, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(string), string);

		string = "WM_PROTOCOLS";
		xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, true, strlen(string), string);
		xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, NULL);

		xcb_atom_t WM_PROTOCOLS = reply->atom;
		free(reply);

		xcb_change_property(connection, XCB_PROP_MODE_APPEND, winID, WM_PROTOCOLS, XCB_ATOM_ATOM, 32, 1, (void*) &WM_DELETE_WINDOW);

		fullscreen = (bool) Client::config.get("fullscreen")->val;
		if (fullscreen) setFullscreen(true);

		context = glXCreateContext(display, visualInfo, NULL, true);
		XFree(visualInfo);

		if (!context) {

			xcb_destroy_window(connection, winID);
			XCloseDisplay(display);

			fputs("Error creating context: an unknown error has occurred\n", stderr);
			return false;

		}

		return true;

	}

	bool initContext() {

		if (!glXMakeCurrent(display, winID, context)) {

			glXDestroyContext(display, context);
			xcb_destroy_window(connection, winID);

			XCloseDisplay(display);
			fputs("Error initalizing context: an unknown error has occurred\n", stderr);

			return false;

		}

		vSync = (bool) Client::config.get("vsync")->val;
		if (!vSync) {

			puts("Warning: VSync is disabled");
			return true;

		}

		PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalEXT");

		if (glXSwapIntervalEXT) {

			glXSwapIntervalEXT(display, winID, vSync);
			return true;

		}

		puts("Warning: extention glXSwapIntervalEXT not available - trying MESA version instead");
		PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalMESA");

		if (glXSwapIntervalMESA) {

			glXSwapIntervalMESA(vSync);
			return true;

		}

		puts("Warning: extention glXSwapIntervalMESA not available - trying SGI version instead");
		PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalSGI");

		if (glXSwapIntervalSGI) {

			glXSwapIntervalSGI(vSync);
			return true;

		}

		puts("Warning: no swap control extentions available; VSync is disabled");
		return true;

	}

	void cleanupContext() {

		glXMakeContextCurrent(display, None, None, NULL);

	}

	void tick() {

		xcb_generic_event_t *event = xcb_wait_for_event(connection);

		if (event) {

			uint8_t type = event->response_type & ~0x80;
			switch (type) {

				case XCB_CLIENT_MESSAGE:

					if (((xcb_client_message_event_t*) event)->data.data32[0] == WM_DELETE_WINDOW) Client::running = false;

				break;

				case XCB_CONFIGURE_NOTIFY:

					width = ((xcb_configure_notify_event_t*) event)->width;
					height = ((xcb_configure_notify_event_t*) event)->height;

					aspect = (float) width / (float) height;

					GFX::resized = true;

				break;


				case XCB_BUTTON_PRESS:
				case XCB_KEY_PRESS: {

					xcb_keycode_t key = ((xcb_key_press_event_t*) event)->detail;
					for (size_t i = 0; i < A_NUM_ACTIONS; i++) {

						if (key == keys[i].key) {

							keys[i].callback();
							keys[i].state = true;
							break;

						}

					}

				} break;

				case XCB_BUTTON_RELEASE:
				case XCB_KEY_RELEASE: {

					xcb_keycode_t key = ((xcb_key_press_event_t*) event)->detail;
					for (size_t i = 0; i < A_NUM_ACTIONS; i++) {

						if (key == keys[i].key) {

							keys[i].state = false;
							break;

						}

					}

				} break;

				case XCB_MOTION_NOTIFY:

					uint16_t x = ((xcb_motion_notify_event_t*) event)->event_x;
					uint16_t y = ((xcb_motion_notify_event_t*) event)->event_y;

					if (width > height) {

						float dX = width - height;
						mouse.x = (((x - (dX / 2)) / (width - dX)) - 0.5f)  *20;

					} else {

						mouse.x = ((x / (float) width) - 0.5f)  *20;

					}

					if (height > width) {

						float dY = height - width;
						mouse.y = ((-((y - (dY / 2)) / (height - dY))) + 0.5f)  *20;

					} else {

						mouse.y = (-((y / (float) height) - 0.5f))  *20;

					}

				break;

			}

		}

		free(event);

	}

	//input section
	Key keys[A_NUM_ACTIONS];
	Point mouse;

	void initInput() {

		keys[A_UP].callback = &actnUp;
		keys[A_DOWN].callback = &actnDown;
		keys[A_LEFT].callback = &actnLeft;
		keys[A_RIGHT].callback = &actnRight;
		keys[A_EXIT].callback = &actnExit;
		keys[A_ACTION].callback = &actnAction;
		keys[A_FULLSCREEN].callback = &actnFullscreen;

		loadKeys();

	}

	void loadKeys() {

		Config config;

		//default values
		config.add("up", (void*) "W");
		config.add("down", (void*) "S");
		config.add("left", (void*) "A");
		config.add("right", (void*) "D");

		config.add("exit", (void*) "Escape");
		config.add("action", (void*) "Left_Click");
		config.add("fullscreen", (void*) "F11");

		config.load("cfg/keymap.cfg");

		KeySym ks;
		char *string;

		for (size_t i = 0; i < config.len; i++) {

			string = (char*) config.get(i)->val;
			ks = XStringToKeysym(string);

			if (ks) keys[i].key = (xcb_keycode_t) XKeysymToKeycode(display, ks);
			else {

				if (!strcmp(string, "Left_Click")) keys[i].key = 1;

			}

		}

	}

	void actnUp() {



	}

	void actnDown() {



	}

	void actnLeft() {



	}

	void actnRight() {



	}

	void actnExit() {

		if (Client::state != Client::PAUSED)
			Client::setState(Client::PAUSED);

	}

	void actnAction() {



	}

	void actnFullscreen() {

		setFullscreen(!fullscreen);

	}

	//OS section
	xcb_connection_t *connection;
	GLXContext context;
	Display  *display;
	xcb_window_t rootWin;
	xcb_window_t winID;
	xcb_atom_t NET_WM_STATE;
	xcb_atom_t NET_WM_STATE_FULLSCREEN;
	xcb_atom_t WM_DELETE_WINDOW;

	bool initX11() {

		char *displayVar = getenv("DISPLAY");
		display = XOpenDisplay(displayVar);

		if (!display) {

			fprintf(stderr, "Error opening display '%s': an unknown error has occurred.\n", displayVar);
			return false;

		}

		int major;
		int minor;

		glXQueryVersion(display, &major, &minor);

		if (major == 1 && minor < 2) {

			XCloseDisplay(display);
			fprintf(stderr, "Error creating visual: glx version 1.2 or greater is required.\n");

			return false;

		}

		connection = XGetXCBConnection(display);
		xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

		screenWidth = screen->width_in_pixels;
		screenHeight = screen->height_in_pixels;

		printf("Detected resolution of: %ix%i\n", screenWidth, screenHeight);

		rootWin = screen->root;

		char *name;
		xcb_intern_atom_cookie_t cookie;
		xcb_intern_atom_reply_t *reply;

		name = "WM_DELETE_WINDOW";
		cookie = xcb_intern_atom(connection, true, strlen(name), name);
		reply = xcb_intern_atom_reply(connection, cookie, NULL);

		WM_DELETE_WINDOW = reply->atom;
		free(reply);

		name = "_NET_WM_STATE";
		cookie = xcb_intern_atom(connection, true, strlen(name), name);
		reply = xcb_intern_atom_reply(connection, cookie, NULL);

		NET_WM_STATE = reply->atom;
		free(reply);

		name = "_NET_WM_STATE_FULLSCREEN";
		cookie = xcb_intern_atom(connection, true, strlen(name), name);
		reply = xcb_intern_atom_reply(connection, cookie, NULL);

		NET_WM_STATE_FULLSCREEN = reply->atom;
		free(reply);

		return true;

	}

}
