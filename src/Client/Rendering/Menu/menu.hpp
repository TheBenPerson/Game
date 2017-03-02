#ifndef HG_Client_Menu_H

#include <GL/gl.h>
#include <stddef.h>
#include "Button/button.hpp"
#include "Util/NodeList/nodelist.hpp"

namespace Menu {

	extern bool active;
	extern bool inGame;

	extern GLuint background;
	extern size_t menuLen;
	extern Button* panel;

	extern Button mainMenu[];
	void settingsProc();
	void quitProc();

	extern Button settings[];
	void backProc();
	void fullscreenProc();

	void init();
	void cleanup();
	void draw();

}

#define HG_Client_Menu_H
#endif
