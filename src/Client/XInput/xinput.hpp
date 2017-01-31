#ifndef HG_Client_XInput_H

#include "Util/Point/Point.hpp"

namespace XInput {

	extern void (* KeyCallbacks[256])();
	extern bool MousePress;
	extern Point MousePos;

	void Init();

}

#define HG_Client_XInput_H
#endif
