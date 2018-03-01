#include <math.h>
#include <stdlib.h>

#include "client.hh"
#include "console.hh"
#include "entity.hh"
#include "net.hh"
#include "packet.hh"
#include "point.hh"
#include "person.hh"
#include "tile.hh"
#include "world.hh"

NodeList Person::persons;

static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		Net::listeners.add((void*) &tickNet);

		cputs(GREEN, "Loaded module: 'person.so'");
		return true;

	}

	void cleanup() {

		Net::listeners.rem((void*) &tickNet);

		cputs(YELLOW, "Unloaded module: 'person.so'");

	}

}

bool tickNet(Packet *packet, Client *client) {

	switch (packet->id) {

		case P_KICK:
		case P_DENY: {

			Person *person = Person::get(client);
			delete person;

		} break;

		case P_NEWP: new Person(client);
		return true;

		case P_UPDP: {

			struct Data {

				__attribute__((packed)) Point pos;
				__attribute__((packed)) Point vel;

			} __attribute__((packed)) *data = (Data*) packet->data;

			Entity *entity = Person::get(client);
			if (!entity) return true;

			entity->pos = data->pos;
			entity->vel = data->vel;
			entity->update();

		} return true;

	}

	return false;

}

Person* Person::get(Client *client) {

	for (unsigned int i = 0; i < persons.len; i++) {

		Person *person = (Person*) persons.get(i);
		if (person->client == client) return person;

	}

	return NULL;

}

Person::Person(Client *client): client(client) {

	type = "person";
	dim = {1, 1};

	interval = 1000;

	add();
	persons.add((void*) this);

}

Person::~Person() {

	persons.rem((void*) this);

}

bool Person::tick(timespec *time) {

	return false;

}
