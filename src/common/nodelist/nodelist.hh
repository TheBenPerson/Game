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

#ifndef GAME_COMMON_NODELIST

#include <limits.h>

#include "timing.hh"

class NodeList {

	public:

		typedef struct Node {

			Node *prev;
			Node *next;

			void *val;

		} Node;

		unsigned int len;

		void* add(void *item, unsigned int index = UINT_MAX);
		void* get(unsigned int index);
		Node* find(void* item); // inspect me
		void rem(unsigned int index);
		void rem(void* item);

		NodeList();
		~NodeList();

	private:

		Timing::mutex m = MTX_DEFAULT;

		Node *last;
		Node *root;

		Node* find(unsigned int index);
		void del(Node* node);

};

#define GAME_COMMON_NODELIST
#endif