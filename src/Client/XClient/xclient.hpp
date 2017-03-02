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
