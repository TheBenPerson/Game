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

#include <stddef.h>

#include "nodelist.hpp"

void* NodeList::add(void *item) {

	Node *node = new Node();

	Timing::lock(&m);

	if (index) last->next = node;
	else index = node;

	node->val = item;

	node->prev = last;
	node->next = NULL;

	last = node;

	len++;

	Timing::unlock(&m);
	return item;

}

NodeList::Node* NodeList::find(unsigned int index) {
	Node* node;

	unsigned int i;

	// find most efficient way to cycle
	if ((len - 1 - index) >= index) {

		node = this->index;

		for (i = 0; i < index; i++)
			node = node->next;

	} else {

		node = last;

		for (i = len - 1; i > index; i--)
			node = node->prev;

	}

	return node;

}

NodeList::Node* NodeList::find(void* item) {

	Node* node = this->index;

	for (;;) {

		if (node->val == item)
			return node;

		if (!node->next) break;
		node = node->next;

	}

	return NULL;

}

void NodeList::del(Node* node) {

	Timing::lock(&m);

	if (node->prev) node->prev->next = node->next;
	else index = node->next;

	if (node->next) node->next->prev = node->prev;

	delete node;
	len--;

	Timing::unlock(&m);

};

void* NodeList::get(unsigned int index) {

	return find(index)->val;

}

void NodeList::rem(unsigned int index) {

	del(find(index));

}

void NodeList::rem(void* item) {

	del(find(item));

}

NodeList::NodeList() : len(0), last(NULL), index(NULL) {}

NodeList::~NodeList() {

	Node* node;

	while (index) {

		node = index;

		index = node->next;
		delete node;

	}

}
