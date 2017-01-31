#include <GL/glx.h>
#include <GL/glxext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "Rendering/rendering.hpp"
#include "XClient/xclient.hpp"
#include "XInput/xinput.hpp"

GLXcontext XClient::context;
GLXWindow XClient::drawable;
bool XClient::fullscreen = true;
unsigned int XClient::height;
Window XClient::hWindow;
Display * XClient::display = NULL;
Window XClient::rootWindow;
unsigned int XClient::screenHeight;
unsigned int XClient::screenWidth;
unsigned int XClient::width;
Atom XClient::XA_NET_WM_STATE;
Atom XClient::XA_NET_WM_STATE_FULLSCREEN;
Atom XClient::XA_WM_DELETE_WINDOW;

void XClient::cleanup() {

	glXMakecontextCurrent(display, None, None, NULL);

    glXDestroycontext(display, context);
    XDestroyWindow(display, hWindow);

    XCloseDisplay(display);

}

bool XClient::createWindow() {

	char* displayVar = getenv("DISPLAY");
	display = XOpenDisplay(displayVar);

	if (!display) {

		fprintf(stderr, "Error opening display %s: an unknown error has occurred.\n", DisplayVar);
		return false;

	}

	int major = 0;
	int minor = 0;

	glXQueryVersion(display, &major, &minor);

	if (major == 1 && minor < 2) {

		XCloseDisplay(display);
		fprintf(stderr, "Error creating visual: glx version 1.2 or greater is required.\n");

		return false;

	}

	int attr[] = { GLX_RGBA,
						 GLX_RED_SIZE, 8,
						 GLX_GREEN_SIZE, 8,
						 GLX_BLUE_SIZE, 8,
						 GLX_ALPHA_SIZE, 8,
						 GLX_DEPTH_SIZE, 24,
						 GLX_DOUBLEBUFFER, 0 };

	XvisualInfo* visualInfo = glXChooseVisual(display, XDefaultScreen(display), attr);

	if (!visualInfo) {

		XCloseDisplay(display);
		fprintf(stderr, "Error choosing visual: no visuals exist that match the required criteria.");

		return false;

	}

	rootWindow = XrootWindow(display, visualInfo->screen);

	screenWidth = XDisplayWidth(display, visualInfo->screen);
	screenHeight = XDisplayHeight(display, visualInfo->screen);

	XSetwinAttr winAttr;

	winAttr.background_pixel = BlackPixel(display, visualInfo->screen);
	winAttr.border_pixel = BlackPixel(display, visualInfo->screen);
	winAttr.colormap = XDefaultColormap(display, visualInfo->screen);
	winAttr.event_mask = ButtonPressMask | ButtonReleaseMask | ExposureMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

	hWindow = XCreateWindow(display, rootWindow, (screenWidth - screenHeight) / 2, 0, screenHeight, screenHeight, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWBackPixel | CWBorderPixel | CWColormap | CWeventMask, &winAttr);

	XA_NET_WM_STATE = XInternAtom(display, "_NET_WM_STATE", true);

	Atom XA_NET_WM_STATE_MAXIMIZED_VERT = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", true);
	XChangeProperty(display, hWindow, XA_NET_WM_STATE, XA_ATOM, 32, PropModeAppend, (const unsigned char*) &XA_NET_WM_STATE_MAXIMIZED_VERT, 1);

	XA_NET_WM_STATE_FULLSCREEN = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);
	//XChangeProperty(display, hWindow, XA_NET_WM_STATE, XA_ATOM, 32, PropModeAppend, (const unsigned char*) &XA_NET_WM_STATE_FULLSCREEN, 1);

	XStoreName(display, hWindow, "Game");

	XA_WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", true);

	XSetWMProtocols(display, hWindow, &XA_WM_DELETE_WINDOW, 1);

	context = glXCreatecontext(display, visualInfo, NULL, true);

	if (!context) {

		XDestroyWindow(display, hWindow);
		XFree(visualInfo);

		XCloseDisplay(display);

		printf("Error creating context: an unknown error has occurred.\n");

		return false;

	}

	XFree(visualInfo);

    return true;

}

bool XClient::finalizecontext() {

	if (!glXMakeCurrent(display, hWindow, context)) {

		glXDestroycontext(display, context);
		XDestroyWindow(display, hWindow);

		XCloseDisplay(display);
		printf("Error initalizing context: an unknown error has occurred.\n");

		return false;

	}

	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalEXT");

	if (glXSwapIntervalEXT) {

		glXSwapIntervalEXT(XClient::display, XClient::hWindow, 1);

	} else {

		printf("Warning: extention glXSwapIntervalEXT not available - trying MESA version instead.\n");

		PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalMESA");

		if (glXSwapIntervalMESA) {

			glXSwapIntervalMESA(1);

		} else {

			printf("Warning: extention glXSwapIntervalMESA not available - trying SGI version instead.\n");

			PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC) glXGetProcAddressARB((const GLubyte *) "glXSwapIntervalSGI");

			if (glXSwapIntervalSGI) {

					glXSwapIntervalSGI(1);

			} else {

				printf("Warning: no swap control extentions available.\n");

			}

		}

	}

	return true;

}

void XClient::messagePump() {

	Xevent event;

	while (!(event.type == ClientMessage && *event.xclient.data.l == XA_WM_DELETE_WINDOW)) {

		XNextevent(XClient::display, &event);

		switch (event.type) {

			case ButtonPress:

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

				Rendering::Resized = true;

			break;

			case KeyPress: {

				char KeyCode;

				XLookupString(&event.xkey, &KeyCode, 1, NULL, NULL);

				if (XInput::KeyCallbacks[KeyCode])
					XInput::KeyCallbacks[KeyCode]();

			}
			break;

			case MotionNotify:

				if (width > height) {

					float dX = width - height;
					XInput::mousePos.x = (((event.xmotion.x - (dX / 2)) / (width - dX)) - 0.5f) * 20;

				} else {

					XInput::mousePos.x = ((event.xmotion.x / (float) width) - 0.5f) * 20;

				}

				if (height > width) {

					float dY = height - width;
					XInput::mousePos.y = ((-((event.xmotion.y - (dY / 2)) / (height - dY))) + 0.5f) * 20;

				} else {

					XInput::mousePos.y = (-((event.xmotion.y / (float) height) - 0.5f)) * 20;

				}

			break;

		}

	}

}

void XClient::setFullscreen(bool mode) {

	XClientMessageevent event;

	memset(&event, 0, sizeof(XClientMessageevent));

	event.type = ClientMessage;
	event.window = hWindow;
	event.message_type = XA_NET_WM_STATE;
	event.format = 32;
	event.data.l[0] = mode;
	event.data.l[1] = XA_NET_WM_STATE_FULLSCREEN;
	event.data.l[2] = 0;
	event.data.l[3] = 1;
	event.data.l[4] = 0;

	fullscreen = mode;

	XSendevent(display, rootWindow, false, SubstructureRedirectMask | SubstructureNotifyMask, (Xevent*) &event);

}
