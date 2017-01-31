#ifndef HG_Util_NodeList_H

class NodeList {

	public:

		unsigned int length;

		void* get(unsigned int index);
		void* pushBack(void* item);
		void remove(unsigned int index);

		~NodeList();

	private:

		struct Node {

			Node* lastNode;
			Node* nextNode;

			void* value;

		};

		Node* finalNode;
		Node* index;

		Node* find(unsigned int index);

};

#define HG_Util_NodeList_H
#endif
