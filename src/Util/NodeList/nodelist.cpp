#include <stddef.h>
#include "nodelist.hpp"

void* NodeList::add(void* item) {

	Node* node = new Node();

	if (index) last->next = node;
	else index = node;

	node->val = item;

	node->prev = last;
	node->next = NULL;

	last = node;

	len++;

}

NodeList::Node* NodeList::find(size_t index) {

	Node* node = this->index;

	for (size_t i = 0; i < index; i++)
		node = node->next;

	return node;

}

void* NodeList::get(size_t index) {

	return find(index)->val;

}

void NodeList::rem(size_t index) {

	Node* node = find(index);

	if (index) node->prev->next = node->next;
	else this->index = node->next;

	if (node->next) node->next->prev = node->prev;

	delete node;

	len--;

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
