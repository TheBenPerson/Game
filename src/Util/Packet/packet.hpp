#ifndef HG_Util_Packet_H

#include <netinet/in.h>

#define P_MAX_SIZE 256

#define P_INF 0x0
#define P_REQ 0x80

#define P_ACK 0x0
#define P_ACT 0x1

#define P_CON 0x0

struct Packet {

	sockaddr_in Address;

	union {

		char Data[P_MAX_SIZE - 1];
		unsigned char Id;
		char Raw[P_MAX_SIZE];

	};

};

#define HG_Util_Packet_H
#endif
