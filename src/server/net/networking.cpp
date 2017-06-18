/*

Game Development Build
https://github.com/TheBenPerson/Game

Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <arpa/inet.hpp>
#include <errno.hpp>
#include <fcntl.hpp>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Client.hpp"
#include "Networking.hpp"
#include "server/Server.hpp"
#include "packet/Packet.hpp"

bool Networking::Running = false;
int Networking::Socket;
pthread_t Networking::Thread;

void Networking::Cleanup() {

	close(Socket);

}

void * Networking::PacketHandler(void *) {

	Packet tPacket;

	while (Running) {

		socklen_t Len = sizeof(sockaddr_in);

		int Ready = recvfrom(Socket, tPacket.Raw, P_MAX_SIZE, 0, (sockaddr *) &tPacket.Address, &Len);

		if (Ready != -1) {

			if (tPacket.Id & 0x80) {

				switch (tPacket.Id & 0x7F) {

					case P_CON:

						tPacket.Id = P_INF | P_ACT;
						send(Socket, &tPacket.Id, 1, 0);

						printf("Client %s connected.\n", inet_ntoa(tPacket.Address.sin_addr));

				}

			} else {

				switch (tPacket.Id & 0x7F) {



				}

			}

		} else {

			usleep(10000);

		}

	}

}

bool Networking::Init(uint16_t Port) {

	Socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (Socket == -1) {

		fprintf(stderr, "Error creating socket: %s.\n", strerror(errno));
		return false;

	}

	sockaddr_in SockAddr;

	memset(&SockAddr, 0, sizeof(sockaddr_in));

	SockAddr.sin_addr.s_addr = INADDR_ANY;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(Port);

	if (bind(Socket, (sockaddr *) &SockAddr, sizeof(sockaddr_in)) == -1) {

		fprintf(stderr, "Error binding socket: %s.\n", strerror(errno));
		Cleanup();

		return false;

	}

	fcntl(Socket, F_SETFL, fcntl(Socket, F_GETFL) | O_NONBLOCK);

	return true;

}

void Networking::Start() {

	Running = true;

	pthread_attr_t Attrib;
	pthread_attr_init(&Attrib);

	pthread_create(&Thread, &Attrib, PacketHandler, NULL);

	pthread_attr_destroy(&Attrib);

}

void Networking::Stop() {

	Running = false;
	pthread_join(Thread, NULL);

	//Disconnect all clients

}
