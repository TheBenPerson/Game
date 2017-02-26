#ifndef HG_Client_XInput_H

#include "Util/Point/point.hpp"

namespace XInput {

	struct Key {

		char key;
		bool state;
		void (*callback)();

	};

	enum {F11, LMOUSE};

	extern Key* keys[];
	extern Point mousePos;

	void cleanup();
	void init();
	void onF11();

}

#define HG_Client_XInput_H
#endif
