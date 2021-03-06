/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2018 Ben Stockett <thebenstockett@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef GAME_SERVER_CLIENT
#define GAME_SERVER_CLIENT

#include <netinet/in.h>

#include "nodelist.hh"
#include "packet.hh"
#include "timing.hh"

class Client {

	public:

		static NodeList clients;
		static uint timeout;

		static Client* get(sockaddr_in *addr);
		static Client* get(char *name);
		static void* entry(void*);
		static void broadcast(Packet *packet, NodeList *clients);
		static void broadcast(Packet *packet);

		sockaddr_in addr;
		char *name;
		bool running = true;

		Client(sockaddr_in *addr, char *name);
		~Client();

		void send(uint8_t id);
		void send(Packet *packet);
		void recv(Packet *packet);
		void kick(char *reason);

	private:

		Packet *packet;
		Timing::thread t;
		Timing::Condition condition;

		Packet* recv();

};

#endif
