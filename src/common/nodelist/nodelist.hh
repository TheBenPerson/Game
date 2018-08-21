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
#define GAME_COMMON_NODELIST

#include <stdint.h>
#include <limits.h>

#ifdef NODELIST_VECTOR
#include <vector>
#endif

#include "timing.hh"

class NodeList {

	public:

		#ifndef NODELIST_VECTOR
		typedef struct Node {

			Node *prev;
			Node *next;

			uintptr_t val;

		} Node;
		#endif

		unsigned int size = 0;

		NodeList();
		~NodeList();

		uintptr_t operator[](unsigned int index);
		Node* find(uintptr_t item); // inspect me

		void add(uintptr_t item, unsigned int index = UINT_MAX);
		void rem(unsigned int index);
		void rem(uintptr_t item);

	private:

		Timing::mutex m = MTX_DEFAULT;

		#ifdef NODELIST_VECTOR

		std::vector<uintptr_t> vector;

		#else

		Node *last = NULL;
		Node *root = NULL;

		Node* find(unsigned int index);
		void del(Node* node);

		#endif

};

#endif
