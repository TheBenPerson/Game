#ifndef HG_Client_Menu_H

#include "Button/button.hpp"
#include "Util/NodeList/nodelist.hpp"

namespace Menu {

	extern bool active;
	extern NodeList panels[];
	extern bool inGame;
	extern unsigned char mode;

	void cleanup();
	void init();

}

#define HG_Client_Menu_H
#endif
