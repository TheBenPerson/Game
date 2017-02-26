#ifndef HG_Client_Rendering_H

#include <GL/gl.h>
#include <pthread.h>
#include "Util/Point/point.hpp"

namespace Rendering {

	extern GLuint font;
	extern bool resized;
	extern pthread_t thread;

	void cleanup();
	void draw();
	void drawText(char* text, Point position, float size, bool centered);
	void glCleanup();
	void glInit();
	bool init();
	GLuint loadTexture(const char * path);
	void* threadMain(void* result);

}

#define HG_Client_Rendering_H
#endif
