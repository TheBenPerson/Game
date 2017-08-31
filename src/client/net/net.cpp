#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "net.hpp"
#include "packet/packet.hpp"

namespace Net {

	bool init() {

		sock = socket(AF_INET, SOCK_DGRAM, 0);

		if (sock == -1) {

			perror("Error creating socket");
			return false;

		}

	}

	void cleanup() {

		close(sock);

	}

	void send(sockaddr_in *addr, Packet *packet) {

		sendto(sock, packet->raw, packet->size, NULL, (sockaddr*) addr, sizeof(sockaddr_in));

	}

	void recv(sockaddr_in *addr, Packet *packet) {

		socklen_t len = sizeof(sockaddr_in);
		packet->size = recvfrom(sock, packet->raw, P_MAX_SIZE, 0, (sockaddr*) addr, &len);

	}

}
