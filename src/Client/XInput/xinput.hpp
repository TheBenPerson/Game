#ifndef HG_Client_XInput_H

#include "Util/Point/point.hpp"

namespace XInput {

	extern void (* keyCallbacks[256])();
	extern bool mousePress;
	extern Point mousePos;

	void init();

}

#define HG_Client_XInput_H
#endif
