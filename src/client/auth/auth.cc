#include "auth.hh"
#include "client.hh"
#include "net.hh"
#include "packet.hh"
#include "timing.hh"

static bool tickNet(Packet *packet);

static Timing::Condition condition;

extern "C" {

	bool init() {

		Net::listeners.add((uintptr_t) &tickNet);

		for (;;) {

			Net::send(P_AUTH);
			bool result = Timing::waitFor(&condition, 1000);
			if (result) break;

		}

		return true;

	}

	void cleanup() {

		Net::listeners.rem((uintptr_t) &tickNet);

	}

}

bool tickNet(Packet *packet) {

	if (packet->id != P_AUTH) return false;

	Timing::signal(&condition);
	return true;

}
