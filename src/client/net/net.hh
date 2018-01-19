#ifndef GAME_CLIENT_NET

#include <stdint.h>

#include "nodelist.hh"
#include "packet.hh"

namespace Net {

	extern NodeList listeners;

	bool start(char *name, char *host, uint16_t port);
	void stop();

	void send(uint8_t id);
	void send(Packet *packet);

}

#define GAME_CLIENT_NET
#endif
