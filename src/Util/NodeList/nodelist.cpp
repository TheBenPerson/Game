#include <cstddef>
#include "nodelist.hpp"

NodeList::Node* NodeList::find(unsigned int index) {

	Node* pNode = this->index;

	for (unsigned int i = 0; i < index; i++) {

		pNode = pNode->nextNode;

	}

	return pNode;

}

void* NodeList::get(unsigned int index) {

	return find(index)->value;

}

void* NodeList::pushBack(void* item) {

	Node* pNode;

	if (!index) {

		pNode = (index = new Node());

	} else {

		pNode = (finalNode->nextNode = new Node());

	}

	pNode->value = item;

	pNode->lastNode = finalNode;
	pNode->nextNode = NULL;

	finalNode = pNode;

	length++;

}

void NodeList::remove(unsigned int index) {

	Node* pNode = find(index);

	if (index) {

		pNode->lastNode->nextNode = pNode->nextNode;

	} else {

		this->index = pNode->nextNode;

	}

	if (pNode->nextNode)
		pNode->nextNode->lastNode = pNode->lastNode;

	delete pNode;

	length--;

}

NodeList::~NodeList() {

	Node* pNode;

	for (unsigned int i = 0; i < length; i++) {

		pNode = index;
		index = index->nextNode;

		delete pNode;

	}

}
