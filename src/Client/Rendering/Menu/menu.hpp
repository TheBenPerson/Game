#ifndef HG_Client_Menu_H

#include <GL/gl.h>
#include "Button/button.hpp"
#include "Util/NodeList/nodelist.hpp"

namespace Menu {

	extern bool active;
	extern GLuint background;
	extern bool inGame;
	extern Button* mainMenu[];
	extern Button** panel;
	extern Button* settings[];

	void cleanup();
	void draw();
	void init();

}

#define HG_Client_Menu_H
#endif
