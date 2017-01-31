#ifndef HG_Client_Rendering_Window_H

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace XClient {

	extern GLXContext context;
	extern GLXWindow drawable;
	extern bool fullscreen;
	extern unsigned int height;
	extern Window hWindow;
	extern Display* display;
	extern Window rootWindow;
	extern unsigned int screenHeight;
	extern unsigned int screenWidth;
	extern XVisualInfo* visualInfo;
	extern unsigned int width;
	extern Atom XA_NET_WM_STATE;
	extern Atom XA_NET_WM_STATE_FULLSCREEN;
	extern Atom XA_WM_DELETE_WINDOW;

	void cleanup();
	bool createWindow();
	bool finalizeContext();
	void messagePump();
	void setFullscreen(bool mode);

}

#define HG_Client_Rendering_Window_H
#endif
