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

#include <GL/glx.h>
#include <GL/glxext.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

#include "client.hpp"
#include "console.hpp"
#include "input.hpp"
#include "config.hpp"
#include "gfx.hpp"
#include "timing.hpp"
#include "win.hpp"

// global variables

namespace WIN {

	bool fullscreen;
	unsigned int screenWidth;
	unsigned int screenHeight;
	bool vSync;

	unsigned int height;
	unsigned int width;
	float aspect;
	bool resized;

}

// static variables
static Timing::thread t;
static bool running = true;;

static xcb_connection_t *connection;
static GLXContext context;
static Display  *display;
static xcb_window_t rootWin;
static xcb_window_t winID;
static xcb_atom_t NET_WM_STATE;
static xcb_atom_t NET_WM_STATE_FULLSCREEN;
static xcb_atom_t WM_DELETE_WINDOW;

static xcb_keysym_t* keys[Input::A_NUM_ACTIONS];

// static functions

static bool initX11();
static bool createWindow();
static bool createContext();
static void* threadMain(void*);
static void loadKeys();
static void fullscreenHandler();

// global functions
extern "C" {

	char* depends[] = {

		"client.so",
		NULL

	};

	bool init() {

		if (!initX11()) return false;
		if (!createWindow()) return false;
		loadKeys();

		t = Timing::createThread(threadMain, NULL);
		Input::listeners.add((void*) &fullscreenHandler);

		cputs(GREEN, "Loaded module: 'win.so'");

		return true;

	}

	void cleanup() {

		Input::listeners.rem((void*) &fullscreenHandler);

		running = false;
		xcb_destroy_window(connection, winID);
		glXDestroyContext(display, context);
		XCloseDisplay(display);

		Timing::waitFor(t);

		for (unsigned int i = 0; i < Input::A_NUM_ACTIONS; i++)
			delete[] keys[i];

		cputs(YELLOW, "Unloaded module: 'win.so'");

	}

}

namespace WIN {

	void showWindow() {

		xcb_map_window(connection, winID);
		if (fullscreen) setFullscreen(true);

	}

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

	bool initContext() {

		if (!glXMakeCurrent(display, winID, context)) {

			glXDestroyContext(display, context);
			xcb_destroy_window(connection, winID);

			XCloseDisplay(display);
			ceputs(RED, "Error initalizing context: an unknown error has occurred");

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

	void swapBuffers() {

		glXSwapBuffers(display, winID);

	}

}

// static functions
bool initX11() {

	char *displayVar = getenv("DISPLAY");
	display = XOpenDisplay(displayVar);

	if (!display) {

		ceprintf(RED, "Error opening display '%s': an unknown error has occurred\n", displayVar);
		return false;

	}

	int major;
	int minor;

	glXQueryVersion(display, &major, &minor);

	if (major == 1 && minor < 2) {

		XCloseDisplay(display);
		ceputs(RED, "Error creating visual: glx version 1.2 or greater is required");

		return false;

	}

	connection = XGetXCBConnection(display);
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

	WIN::screenWidth = screen->width_in_pixels;
	WIN::screenHeight = screen->height_in_pixels;

	printf("Detected resolution of: %ix%i\n", WIN::screenWidth, WIN::screenHeight);

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
		ceputs(RED, "Error choosing visual: no visuals exist that match the required criteria");

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
	xcb_create_window(connection, visualInfo->depth, winID, rootWin, (WIN::screenWidth - WIN::screenHeight) / 2, 0, WIN::screenHeight, WIN::screenHeight, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, visualInfo->visualid, XCB_CW_EVENT_MASK, &eventMask);

	char *string;

	string = "Game";
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, winID, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(string), string);

	string = "WM_PROTOCOLS";
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, true, strlen(string), string);
	xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, NULL);

	xcb_atom_t WM_PROTOCOLS = reply->atom;
	free(reply);

	xcb_change_property(connection, XCB_PROP_MODE_APPEND, winID, WM_PROTOCOLS, XCB_ATOM_ATOM, 32, 1, (void*) &WM_DELETE_WINDOW);

	WIN::fullscreen = (bool) Client::config.get("fullscreen")->val;
	// set fullscreen after window is mapped
	// some window managers like i3 don't set it fullscreen otherwise

	context = glXCreateContext(display, visualInfo, NULL, true);
	XFree(visualInfo);

	if (!context) {

		xcb_destroy_window(connection, winID);
		XCloseDisplay(display);

		ceputs(RED, "Error creating context: an unknown error has occurred");
		return false;

	}

	return true;

}

void* threadMain(void*) {

	for (;;) {

		xcb_generic_event_t *event = xcb_wait_for_event(connection);
		if (!event && !running) break;

		uint8_t type = event->response_type & ~0x80;

		if (type == XCB_CONFIGURE_NOTIFY) {

			WIN::width = ((xcb_configure_notify_event_t*) event)->width;
			WIN::height = ((xcb_configure_notify_event_t*) event)->height;

			WIN::aspect = (float) WIN::width / (float) WIN::height;
			WIN::resized = true;

		} else if (type == XCB_CLIENT_MESSAGE) {

			if (((xcb_client_message_event_t*) event)->data.data32[0] == WM_DELETE_WINDOW)
				killpg(NULL, SIGINT);

		} else {

			switch (type) {

				case XCB_BUTTON_PRESS:
				case XCB_KEY_PRESS: {

					xcb_keycode_t key = ((xcb_key_press_event_t*) event)->detail;
					for (unsigned int i = 0; i < Input::A_NUM_ACTIONS; i++) {

						for (unsigned int n = 0; keys[i][n]; n++) {

							if (keys[i][n] == key) {

								Input::actions[i] = true;
								break;

							}

						}

					}

					Input::wasCursor = false;

				} break;

				case XCB_BUTTON_RELEASE:
				case XCB_KEY_RELEASE: {

					xcb_keycode_t key = ((xcb_key_press_event_t*) event)->detail;
					for (unsigned int i = 0; i < Input::A_NUM_ACTIONS; i++) {

						for (unsigned int n = 0; keys[i][n]; n++) {

							if (keys[i][n] == key) {

								Input::actions[i] = false;
								break;

							}

						}

					}

					Input::wasCursor = false;

				} break;

				case XCB_MOTION_NOTIFY:

					uint16_t x = ((xcb_motion_notify_event_t*) event)->event_x;
					uint16_t y = ((xcb_motion_notify_event_t*) event)->event_y;

					if (WIN::width > WIN::height) {

						float dX = WIN::width - WIN::height;
						Input::cursor.x = (((x - (dX / 2)) / (WIN::width - dX)) - 0.5f) * 20;

					} else Input::cursor.x = ((x / (float) WIN::width) - 0.5f) * 20;

					if (WIN::height > WIN::width) {

						float dY = WIN::height - WIN::width;
						Input::cursor.y = ((-((y - (dY / 2)) / (WIN::height - dY))) + 0.5f) * 20;

					} else Input::cursor.y = (-((y / (float) WIN::height) - 0.5f)) * 20;

					Input::wasCursor = true;

				break;

			}

			Input::notify();

		}

		free(event);

	}

	return NULL;

}

void loadKeys() {

	Config config;

	// default values
	config.add("up", (void*) "W, Up");
	config.add("down", (void*) "S, Down");
	config.add("left", (void*) "A, Left");
	config.add("right", (void*) "D, Right");

	config.add("exit", (void*) "Escape");
	config.add("action", (void*) "Left_Click, Return");
	config.add("fullscreen", (void*) "F11");

	config.load("cfg/keymap.cfg");

	for (unsigned int i = 0; i < config.len; i++) {

		char *string = strdup((char*) config.get(i)->val);
		// don't need to if only one binding: might want to redesign this

		char *c = string;
		unsigned int n = 1;
		// at least 1 binding /w null terminator

		for (; c - 1; n++)
			c = index(c, ',') + 1;

		keys[i] = new xcb_keysym_t[n];

		c = strtok(string, ", ");

		n = 0;
		while (c) {

			KeySym ks = XStringToKeysym(c);

			if (ks) keys[i][n] = (xcb_keycode_t) XKeysymToKeycode(display, ks);
			else if (!strcmp(string, "Left_Click")) keys[i][n] = 1;
			// find the official key code for left click

			n++;

			c = strtok(NULL, ", ");

		}

		keys[i][++n] = NULL;
		free(string);

	}

}

void fullscreenHandler() {

	if (Input::actions[Input::A_FULLSCREEN]) {

		WIN::fullscreen = !WIN::fullscreen;
		WIN::setFullscreen(WIN::fullscreen);

	}

}
