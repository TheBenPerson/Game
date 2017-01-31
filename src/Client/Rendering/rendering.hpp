#ifndef HG_Client_Rendering_H

#include <GL/gl.h>
#include <pthread.h>
#include "Util/Point/point.hpp"

namespace Rendering {

	extern GLuint font;
	extern bool resized;
	extern pthread_t thread;

	void cleanup();
	void glCleanup();
	void glInit();
	bool init();
	GLuint loadTexture(const char * path);
	void * renderLoop(void* result);
	void drawText(const char* text, Point position, float maxWidth, float size, bool delayed);

}

#define HG_Client_Rendering_H
#endif
