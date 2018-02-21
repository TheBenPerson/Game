#ifndef GAME_SERVER_SIGN
#define GAME_SERVER_SIGN

#include "client.hh"

namespace Sign {

	void send(Client *client, char *string);
	void send(char *string);

}

#endif
