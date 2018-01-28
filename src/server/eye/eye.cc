#include "console.hh"
#include "eye.hh"

extern "C" {

	char* eye_deps[] = {

		"entity.so",
		NULL

	};

	bool init() {

		Point pos = {2.5f, 2.5f};
		Point vel = {-.01f, -.01f};
		new Eye(pos, vel);

		pos = {2.5f, -2.5f};
		vel = {-.01f, .01f};
		new Eye(pos, vel);

		pos = {-2.5f, -2.5f};
		vel = {.01f, .01f};
		new Eye(pos, vel);

		pos = {-2.5f, 2.5f};
		vel = {.01f, -.01f};
		new Eye(pos, vel);

		cputs(GREEN, "Loaded module: 'eye.so'");
		return true;

	}

	void cleanup() {

		cputs(YELLOW, "Unloaded module: 'eye.so'");

	}

}

Eye::Eye(Point pos, Point vel): Entity(pos, vel) {}

void Eye::tick() {}

char* Eye::getType() {

	return "eye";

}
