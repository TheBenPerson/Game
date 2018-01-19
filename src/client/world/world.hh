#ifndef GAME_CLIENT_WORLD

#include <GL/gl.h>

#include "tile.hh"

namespace World {

	extern GLuint tex;
	extern unsigned int width;
	extern unsigned int height;
	extern Tile *tiles;

	void init();
	void cleanup();
	void draw();

}

#define GAME_CLIENT_WORLD
#endif
