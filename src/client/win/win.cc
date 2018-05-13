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

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.hh"
#include "console.hh"
#include "input.hh"
#include "config.hh"
#include "gfx.hh"
#include "main.hh"
#include "timing.hh"
#include "win.hh"

namespace WIN {

	bool fullscreen;
	unsigned int screenWidth = 640;
	unsigned int screenHeight = 480;
	bool vSync;

	unsigned int height;
	unsigned int width;
	float aspect;
	bool resized;

}

static GLFWwindow* window;
static int* keys[Input::NUM_ACTIONS];
static Timing::thread t;
static bool running = true;
static char **text;

static bool createWindow();
static void* threadMain(void*);
static void loadKeys();
static void toggleFS();
static void fullscreenHandler();

static void eventHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

static void errorHandler(int, const char *error) {

	ceprintf(RED, "GLFW error: %s\n", error);

}

extern "C" {

	bool init() {

		glfwSetErrorCallback(errorHandler);
		if (!glfwInit()) return false;

		Client::config.set("win.fullscreen", (void*) true);
		Client::config.set("win.vsync", (void*) true);

		Client::config.set("win.kbd.exit", (void*) "ESC");
		Client::config.set("win.kbd.fullscreen", (void*) "F11");
		Client::config.set("win.kbd.left", (void*) "A");
		Client::config.set("win.kbd.right", (void*) "D");
		Client::config.set("win.kbd.up", (void*) "W");
		Client::config.set("win.kbd.down", (void*) "S");
		Client::config.set("win.kbd.primary", (void*) "LMOUSE,RETURN");
		Client::config.set("win.kbd.secondary", (void*) "SPACE");
		Client::config.set("win.kbd.modifier", (void*) "LSHIFT");

		Client::config.load("win.cfg");
		if (!createWindow()) return false;

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

		if (running) {

			running = false;
			glfwPostEmptyEvent();

			Timing::waitFor(t);

		}

		glfwDestroyWindow(window);
		glfwTerminate();

		for (unsigned int i = 0; i < Input::NUM_ACTIONS; i++)
			delete[] keys[i];

		Client::config.del("win");

		cputs(YELLOW, "Unloaded module: 'win.so'");

	}

}

namespace WIN {

	void showWindow() {

		glfwShowWindow(window);

		WIN::fullscreen = (bool) Client::config.get("win.fullscreen")->val;
		if (WIN::fullscreen) WIN::setFullscreen(true);

	}

	void setFullscreen(bool mode) {

		GLFWmonitor *monitor = NULL;

		unsigned int width = screenHeight;
		unsigned int height = screenHeight;

		if (mode) {

			monitor = glfwGetPrimaryMonitor();
			width = screenWidth;

		}

		glfwSetWindowMonitor(window, monitor, (screenWidth - screenHeight) / 2, 0, width, height, GLFW_DONT_CARE);

		if (mode) *text = "Fullscreen: True";
		else *text = "Fullscreen: False";

	}

	void setCursor(bool mode) {

		glfwSetInputMode(window, GLFW_CURSOR, mode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

	}

	bool initContext() {

		vSync = (bool) Client::config.get("win.vsync")->val;
		if (!vSync) puts("Warning: VSync is disabled");

		glfwMakeContextCurrent(window);
		glfwSwapInterval(vSync);

		return true;

	}

	void cleanupContext() {

		glfwMakeContextCurrent(NULL);

	}

	void swapBuffers() {

		glfwSwapBuffers(window);

	}

}

static void resizeHandler(GLFWwindow* window, int width, int height) {

	WIN::width = width;
	WIN::height = height;

	WIN::aspect = (float) WIN::width / (float) WIN::height;
	WIN::resized = true;

}

static void cursorHandler(GLFWwindow* window, double posX, double posY) {

	float x = posX;
	float y = posY;
	y = WIN::height - y;

	Input::cursor.x = (((x / WIN::width) * 20) - 10) * WIN::aspect;
	Input::cursor.y = ((y / WIN::height) * 20) - 10;

	Input::wasCursor = true;
	Input::notify();

}

static void mouseHandler(GLFWwindow* window, int button, int action, int mods) {

	eventHandler(window, button, NULL, action, mods);

}

bool createWindow() {

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	if (monitor)  {

		GLFWvidmode *mode = (GLFWvidmode*) glfwGetVideoMode(monitor);
		if (mode) {

			WIN::screenWidth = mode->width;
			WIN::screenHeight = mode->height;

			printf("Detected resolution of (%ix%i)\n", WIN::screenWidth, WIN::screenHeight);

		} else perror("Warning: error getting screen resolution");

	} else perror("Warning: error getting monitor information");

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	window = glfwCreateWindow(WIN::screenHeight, WIN::screenHeight, "Game", NULL, NULL);
	if (!window) return false;

	glfwSetWindowSizeCallback(window, &resizeHandler);
	glfwSetCursorPosCallback(window, &cursorHandler);
	glfwSetMouseButtonCallback(window, &mouseHandler);

	return true;

}

void* threadMain(void*) {

	for (; running;) {

		bool result = glfwWindowShouldClose(window);
		if (result) {

			// signal not to send empty event
			running = false;
			Game::stop();

			break;

		}

		glfwWaitEvents();

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

	keys[action] = new int[n];

	c = strtok(string, ", ");

	for (n = 0; c; n++) {

		if (!strcmp(string, "LSHIFT")) keys[action][n] = GLFW_KEY_LEFT_SHIFT;
		else if (!strcmp(string, "RSHIFT")) keys[action][n] = GLFW_KEY_RIGHT_SHIFT;
		else if (!strcmp(string, "LMOUSE")) keys[action][n] = GLFW_MOUSE_BUTTON_LEFT;
		else if (!strcmp(string, "SPACE")) keys[action][n] = GLFW_KEY_SPACE;
		else if (!strcmp(string, "RETURN")) keys[action][n] = GLFW_KEY_ENTER;
		else keys[action][n] = c[0];

		c = strtok(NULL, ", ");

	}

	keys[action][n] = ~0;
	free(string);

}

void eventHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_REPEAT) return;

	for (unsigned int i = 0; i < Input::NUM_ACTIONS; i++) {

		for (unsigned int j = 0; keys[i][j] != ~0; j++) {

			if (keys[i][j] == key) {

				Input::actions[i].state = (action == GLFW_PRESS);
				Input::actions[i].changed = true;

				Input::wasCursor = false;
				Input::notify();

				Input::actions[i].changed = false;
				return;

			}

		}

	}

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

	glfwSetKeyCallback(window, &eventHandler);

}

void toggleFS() {

	WIN::fullscreen = !WIN::fullscreen;
	WIN::setFullscreen(WIN::fullscreen);

}

void fullscreenHandler() {

	if (Input::actions[Input::FULLSCREEN].changed && Input::actions[Input::FULLSCREEN].state) toggleFS();

}
