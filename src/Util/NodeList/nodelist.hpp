#ifndef HG_Util_NodeList_H

#include <stddef.h>

class NodeList {

	public:

		size_t len;

		void* add(void *item);
		void* get(size_t index);
		void rem(size_t index);

		NodeList();
		~NodeList();

	private:

		struct Node {

			Node *prev;
			Node *next;

			void* val;

		};

		Node *last;
		Node *index;

		Node* find(size_t index);

};

#define HG_Util_NodeList_H
#endif
