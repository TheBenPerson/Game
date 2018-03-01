#ifndef GAME_SERVER_PERSON
#define GAME_SERVER_PERSON

#include "client.hh"
#include "entity.hh"
#include "nodelist.hh"

class Person: public Entity {

	public:

		static NodeList persons;

		static Person* get(Client *client);

		Person(Client *client);
		~Person();

		bool tick(timespec *time);

	private:

		Client *client;

};

#endif
