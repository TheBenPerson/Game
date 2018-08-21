#include "auth.hh"
#include "client.hh"
#include "net.hh"
#include "packet.hh"
#include "world.hh"

static NodeList auths;

static bool tickNet(Packet *packet, Client *client);

extern "C" {

	bool init() {

		Net::listeners.add((uintptr_t) &tickNet);
		return true;

	}

	void cleanup() {

		Net::listeners.rem((uintptr_t) &tickNet);
		while (auths.size) delete (Auth*) auths[0];

	}

}

Auth* Auth::get(Client *client) {

	for (unsigned int i = 0; i < auths.size; i++) {

		Auth *auth = (Auth*) auths[i];
		if (auth->client == client) return auth;

	}

	return NULL;

}

Auth::Auth(Client *client): client(client) {

	auths.add((uintptr_t) this);
	World::defaultWorld->clients.add((uintptr_t) client);

}

Auth::~Auth() {

	World::defaultWorld->clients.rem((uintptr_t) client); // todo: ?
	auths.rem((uintptr_t) this);

}

bool tickNet(Packet *packet, Client *client) {

	if (packet->id != P_AUTH) return false;

	Auth *auth = Auth::get(client);
	if (!auth) auth = new Auth(client);

	client->send(P_AUTH);
	return true;

}
