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

#include <GL/glx.h>
#include <GL/glxext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

#include "client.hh"
#include "console.hh"
#include "input.hh"
#include "config.hh"
#include "gfx.hh"
#include "main.hh"
#include "timing.hh"
#include "win.hh"

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
static bool running = true;

static xcb_connection_t *connection;
static GLXContext context;
static Display  *display;
static xcb_window_t rootWin;
static xcb_window_t winID;
static xcb_atom_t NET_WM_STATE;
static xcb_atom_t NET_WM_STATE_FULLSCREEN;
static xcb_atom_t WM_DELETE_WINDOW;

static xcb_keysym_t* keys[Input::NUM_ACTIONS];

static char **text;

// static functions

static bool initX11();
static bool createWindow();
static void* threadMain(void*);
static void loadKeys();
static void toggleFS();
static void fullscreenHandler();

// global functions
extern "C" {

	bool init() {

		// doesn't require configs and could fail
		if (!initX11()) return false;

		Client::config.set("win.fullscreen", (void*) true);
		Client::config.set("win.vsync", (void*) true);

		Client::config.set("win.kbd.exit", (void*) "Escape");
		Client::config.set("win.kbd.fullscreen", (void*) "F11");
		Client::config.set("win.kbd.left", (void*) "A");
		Client::config.set("win.kbd.right", (void*) "D");
		Client::config.set("win.kbd.up", (void*) "W");
		Client::config.set("win.kbd.down", (void*) "S");
		Client::config.set("win.kbd.primary", (void*) "Left_Click,Return");
		Client::config.set("win.kbd.secondary", (void*) "space");
		Client::config.set("win.kbd.modifier", (void*) "Shift_L");

		Client::config.load("win.cfg");
		if (!createWindow()) return false; // could also fail

		Button::Action action;
		action.menu = NULL;
		action.isMenu = true;

		Button::Menu *menu = (new Button("Window", &action, Client::settings))->action.menu;

		action.callback = (void (*)(Button*)) &toggleFS;
		action.isMenu = false;

		text = &(new Button("Fullscreen: False", &action, menu))->name;

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

		for (unsigned int i = 0; i < Input::NUM_ACTIONS; i++)
			delete[] keys[i];

		Client::config.del("win");

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

		if (mode) *text = "Fullscreen: True";
		else *text = "Fullscreen: False";

	}

	bool initContext() {

		if (!glXMakeCurrent(display, winID, context)) {

			glXDestroyContext(display, context);
			xcb_destroy_window(connection, winID);

			XCloseDisplay(display);
			ceputs(RED, "Error initalizing context: an unknown error has occurred");

			return false;

		}

		vSync = (bool) Client::config.get("win.vsync")->val;
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

	WIN::fullscreen = (bool) Client::config.get("win.fullscreen")->val;
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
				Game::stop();

		} else {

			switch (type) {

				case XCB_BUTTON_PRESS:
				case XCB_KEY_PRESS: {

					xcb_keycode_t key = ((xcb_key_press_event_t*) event)->detail;
					for (unsigned int i = 0; i < Input::NUM_ACTIONS; i++) {

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
					for (unsigned int i = 0; i < Input::NUM_ACTIONS; i++) {

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

					float x = (float) ((xcb_motion_notify_event_t*) event)->event_x;
					float y = (float) ((xcb_motion_notify_event_t*) event)->event_y;
					y = WIN::height - y;

					Input::cursor.x = (((x / WIN::width) * 20) - 10) * WIN::aspect;
					Input::cursor.y = ((y / WIN::height) * 20) - 10;

					Input::wasCursor = true;

				break;

			}

			Input::notify();

		}

		free(event);

	}

	return NULL;

}

static void loadKey(Input::Action action, char *key) {

	char *string = strdup((char*) Client::config.get(key)->val);
	// don't need to if only one binding: might want to redesign this

	char *c = string;
	unsigned int n = 1;
	// at least 1 binding /w null terminator

	for (; c - 1; n++)
		c = strchr(c, ',') + 1;

	keys[action] = new xcb_keysym_t[n];

	c = strtok(string, ", ");

	for (n = 0; c; n++) {

		KeySym ks = XStringToKeysym(c);

		if (ks) keys[action][n] = (xcb_keycode_t) XKeysymToKeycode(display, ks);
		else if (!strcmp(string, "Left_Click")) keys[action][n] = 1;
		// find the official key code for left click

		c = strtok(NULL, ", ");

	}

	keys[action][n] = NULL;
	free(string);

}

void loadKeys() {

	loadKey(Input::EXIT, "win.kbd.exit");
	loadKey(Input::FULLSCREEN, "win.kbd.fullscreen");
	loadKey(Input::LEFT, "win.kbd.left");
	loadKey(Input::RIGHT, "win.kbd.right");
	loadKey(Input::UP, "win.kbd.up");
	loadKey(Input::DOWN, "win.kbd.down");
	loadKey(Input::PRIMARY, "win.kbd.primary");
	loadKey(Input::SECONDARY, "win.kbd.secondary");
	loadKey(Input::MODIFIER, "win.kbd.modifier");

}

void toggleFS() {

	WIN::fullscreen = !WIN::fullscreen;
	WIN::setFullscreen(WIN::fullscreen);

}

void fullscreenHandler() {

	if (Input::actions[Input::FULLSCREEN]) toggleFS();

}
