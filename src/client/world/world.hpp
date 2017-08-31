#include <GL/gl.h>

namespace World {

	extern GLuint tex;
	extern unsigned int width;
	extern unsigned int height;
	extern int *tiles;

	void init();
	void cleanup();
	void draw();

}
