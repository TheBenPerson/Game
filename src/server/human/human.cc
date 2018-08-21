#include <stdlib.h>
#include <string.h>

#include "client.hh"
#include "entity.hh"
#include "point.hh"
#include "human.hh"

extern "C" {

	bool init() {

		return true;

	}

	void cleanup() {}

}

Human::Human(World *world, bool send): Entity(world) {

	name = "Wild Human";

	type = "human";
	dim = {13, 17};
	dim /= dim.y;

	interval = 1000;
	if (send) this->send();

}

bool Human::tick(unsigned int time) {

	return false;

}

void Human::toNet(Packet *packet) {

	packet->size = 1 + SIZE_TENTITY + strlen(name) + 1;
	packet->raw = (uint8_t*) malloc(packet->size);

	packet->raw[0] = P_GENT;
	pack(packet->raw + 1);

	strcpy((char*) packet->raw + 1 + SIZE_TENTITY, name);

}
