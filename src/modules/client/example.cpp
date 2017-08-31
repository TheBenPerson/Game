#include <stdio.h>
#include <stdlib.h>

extern "C" {

	// modules print their own error codes
	bool init() {

		puts("Loaded module: 'example.so'");
		return true;

	}

	void cleanup() {

		puts("Unloaded module: 'example.so'");

	}

	void tick() {



	}

	void draw() {



	}

}
