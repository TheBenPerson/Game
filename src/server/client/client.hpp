/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>
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

#ifndef GAME_SERVER_PLAYER

#include <netinet/in.h>

#include "nodelist/nodelist.hpp"
#include "packet/packet.hpp"
#include "timing/timing.hpp"

class Client {

	public:

		static uint timeout;

		static Client* get(sockaddr_in *addr);
		static Client* get(char *name);
		static void* entry(void*);

		sockaddr_in addr;
		char *name;

		Client(sockaddr_in *addr, char *name);
		~Client();
		void send(uint8_t id);
		void send(Packet *packet);
		void recv(Packet *packet);

	private:

		static NodeList clients;

		Packet *packet;
		Timing::thread t;
		Timing::condition cond;

		Packet* recv();

};

#define GAME_SERVER_PLAYER
#endif
