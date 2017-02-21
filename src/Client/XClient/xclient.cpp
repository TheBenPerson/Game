#include <GL/glx.h>
#include <GL/glxext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include "client.hpp"
#include "Rendering/rendering.hpp"
#include "Util/Config/config.hpp"
#include "XClient/xclient.hpp"
#include "XInput/xinput.hpp"

xcb_connection_t *XClient::connection;
GLXContext XClient::context;
bool XClient::fullscreen = true;
unsigned int XClient::height;
Display  *XClient::display;
Window XClient::rootWindow;
unsigned int XClient::screenHeight;
unsigned int XClient::screenWidth;
bool XClient::vSync;
unsigned int XClient::width;
xcb_window_t XClient::winID;
Atom XClient::XA_NET_WM_STATE;
Atom XClient::XA_NET_WM_STATE_FULLSCREEN;
xcb_intern_atom_cookie_t XClient::XA_WM_DELETE_WINDOW;

void XClient::cleanup() {

	glXMakeContextCurrent(display, None, None, NULL);

    glXDestroyContext(display, context);
    XDestroyWindow(display, winID);

    XCloseDisplay(display);

}

bool XClient::createWindow() {

	char *displayVar = getenv("DISPLAY");
	display = XOpenDisplay(displayVar);

	if (!display) {

		fprintf(stderr, "Error opening display %s: an unknown error has occurred.\n", displayVar);
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

	int attr[] = {

		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_DOUBLEBUFFER, true,
		None

	};

	XVisualInfo *visualInfo = glXChooseVisual(display, 0/*CHANGE THIS!!!*/, attr);

	if (!visualInfo) {

		XCloseDisplay(display);
		fprintf(stderr, "Error choosing visual: no visuals exist that match the required criteria.\n");

		return false;

	}

	connection = XGetXCBConnection(display);
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

	screenWidth = screen->width_in_pixels;
	screenHeight = screen->height_in_pixels;

	printf("Detected resolution of: %ix%i\n", screenWidth, screenHeight);

	uint32_t eventMask = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_POINTER_MOTION;

	winID = xcb_generate_id(connection);
	xcb_create_window(connection, visualInfo->depth, winID, screen->root, (screenWidth - screenHeight) / 2, 0, screenHeight, screenHeight, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, visualInfo->visualid, XCB_CW_EVENT_MASK, &eventMask);

	char *title = "Game";
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, winID, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title), title);

	/*XA_NET_WM_STATE = XInternAtom(display, "_NET_WM_STATE", true);

	XA_NET_WM_STATE_FULLSCREEN = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);
	//XChangeProperty(display, winID, XA_NET_WM_STATE, XA_ATOM, 32, PropModeAppend, (const unsigned char*) &XA_NET_WM_STATE_FULLSCREEN, 1);

	XA_WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", true);
	* XSetWMProtocols(display, winID, &XA_WM_DELETE_WINDOW, 1);*/

	char *name = "WM_PROTOCOLS";
	xcb_intern_atom_cookie_t XA_WM_PROTOCOLS = xcb_intern_atom(connection, true, strlen(name), name);

	name = "WM_DELETE_WINDOW";
	//XA_WM_DELETE_WINDOW = xcb_intern_atom(connection, true, strlen(name), name);
	//xcb_change_property(connection, XCB_PROP_MODE_APPEND, winID, XA_WM_PROTOCOLS, XCB_ATOM, 16, 1, XA_WM_DELETE_WINDOW);

	xcb_flush(connection);

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

bool XClient::finalizeContext() {

	if (!glXMakeCurrent(display, winID, context)) {

		glXDestroyContext(display, context);
		xcb_destroy_window(connection, winID);

		XCloseDisplay(display);
		fputs("Error initalizing context: an unknown error has occurred\n", stderr);

		return false;

	}

	vSync = (bool) Client::config.get("vSync")->val;

	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalEXT");

	if (glXSwapIntervalEXT) {

		glXSwapIntervalEXT(display, winID, vSync);

	} else {

		puts("Warning: extention glXSwapIntervalEXT not available - trying MESA version instead");

		PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalMESA");

		if (glXSwapIntervalMESA) {

			glXSwapIntervalMESA(vSync);

		} else {

			puts("Warning: extention glXSwapIntervalMESA not available - trying SGI version instead");

			PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalSGI");

			if (glXSwapIntervalSGI) {

				glXSwapIntervalSGI(vSync);

			} else {

				if (vSync) puts("Warning: no swap control extentions available");
				vSync = false;

			}

		}

	}

	if (!vSync) puts("Warning: VSync disabled");

	return true;

}

void XClient::messagePump() {

	for (;;) xcb_wait_for_event(connection);

	/*xcb_generic_event_t *event;

	while (!(event.type == ClientMessage && *event.xclient.data.l == XA_WM_DELETE_WINDOW)) {

		event = xcb_wait_for_event(connection);

		switch (event->response_type) {

			case XCB_BUTTON_PRESS:

				if (event.xbutton.button == 1)
					XInput::mousePress = true;

			break;

			case ButtonRelease:

				if (event.xbutton.button == 1)
					XInput::mousePress = false;

			break;

			case ConfigureNotify:

				width = event.xconfigure.width;
				height = event.xconfigure.height;

				Rendering::resized = true;

			break;

			case KeyPress: {

				char keyCode;

				XLookupString(&event.xkey, &keyCode, 1, NULL, NULL);

				if (XInput::keyCallbacks[keyCode])
					XInput::keyCallbacks[keyCode]();

			}
			break;

			case MotionNotify:

				if (width > height) {

					float dX = width - height;
					XInput::mousePos.x = (((event.xmotion.x - (dX / 2)) / (width - dX)) - 0.5f)  *20;

				} else {

					XInput::mousePos.x = ((event.xmotion.x / (float) width) - 0.5f)  *20;

				}

				if (height > width) {

					float dY = height - width;
					XInput::mousePos.y = ((-((event.xmotion.y - (dY / 2)) / (height - dY))) + 0.5f)  *20;

				} else {

					XInput::mousePos.y = (-((event.xmotion.y / (float) height) - 0.5f))  *20;

				}

			break;

		}

	}*/

}

void XClient::setFullscreen(bool mode) {

	XClientMessageEvent event;

	memset(&event, 0, sizeof(XClientMessageEvent));

	event.type = ClientMessage;
	event.window = winID;
	event.message_type = XA_NET_WM_STATE;
	event.format = 32;
	event.data.l[0] = mode;
	event.data.l[1] = XA_NET_WM_STATE_FULLSCREEN;
	event.data.l[2] = 0;
	event.data.l[3] = 1;
	event.data.l[4] = 0;

	fullscreen = mode;

	XSendEvent(display, rootWindow, false, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*) &event);

}
