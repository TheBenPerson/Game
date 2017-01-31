#ifndef HG_Client_Button_H

#include <GL/gl.h>
#include "Util/Point/point.hpp"

class Button {

	public:

		static GLuint texture;

		void (*callback)();
		unsigned char state;
		float values[7];

		Button(Point pos, float Width, void (*Callback)());

		static void cleanup();
		static void init();

		void render();
		void tick();

};

#define HG_Client_Button_H
#endif
