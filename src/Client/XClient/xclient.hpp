#ifndef HG_Client_Rendering_Window_H

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

namespace XClient {

	extern xcb_connection_t *connection;
	extern GLXContext context;
	extern bool fullscreen;
	extern unsigned int height;
	extern Display* display;
	extern Window rootWindow;
	extern unsigned int screenHeight;
	extern unsigned int screenWidth;
	extern bool vSync;
	extern unsigned int width;
	extern xcb_window_t winID;
	extern Atom XA_NET_WM_STATE;
	extern Atom XA_NET_WM_STATE_FULLSCREEN;
	extern xcb_intern_atom_cookie_t XA_WM_DELETE_WINDOW;

	void cleanup();
	bool createWindow();
	bool finalizeContext();
	void messagePump();
	void setFullscreen(bool mode);

}

#define HG_Client_Rendering_Window_H
#endif
