#ifndef HG_Client_Button_H

#include <GL/gl.h>
#include "Util/Point/point.hpp"

class Button {

	public:

		static void cleanup();
		static void init();

		Button(Point pos, float width, char *name, void (*callback)());

		void draw();
		void tick();

	private:

		static GLuint texture;

		void (*callback)();
		char *name;
		enum {normal, hover, clicked} state;
		Point values[2];
		float width;

};

#define HG_Client_Button_H
#endif
