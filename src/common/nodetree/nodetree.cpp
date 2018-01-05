#include <stdlib.h>
#include <string.h>

#include "nodetree.hpp"

NodeTree::NodeTree() {

	root = new Key;
	root->name = strdup("root");
	root->val = new NodeList;
	root->t = key;

}

NodeTree::~NodeTree() {

	del(root);

}

NodeTree::Key* NodeTree::get(char *str) {

	if (!str) return root;

	char *name = strdup(str);
	char *token = strtok(name, ".");

	Key *key = root;

	while (token) {

		key = find(token, (NodeList*) key->val);
		if (!key || key->t != type::key) break;

		token = strtok(NULL, ".");

	}

	free(name);
	return key;

}

void NodeTree::set(char *str, void* val, bool heap) {

	char *name = strdup(str);

	Key *key = root;
	char *token = strtok(name, ".");

	for (;;) {

		NodeList *list = (NodeList*) key->val;
		key = find(token, list);

		if (!key) {

			key = new Key();
			key->name = strdup(token);

			token = strtok(NULL, ".");

			if (token) {

				key->val = new NodeList;
				key->t = type::key;

			} else key->t = type::val;

			list->add((void*) key);

		} else token = strtok(NULL, ".");

		if (!token) break;

	}

	free(name);
	if (key->t == hval) free(val);

	if (heap) key->t = hval;
	else key->t = type::val;

	key->val = val;

}

void NodeTree::del(char *name) {

	Key *key = get(name);
	del(key);

	NodeList *list = (NodeList*) root->val;

	name = strdup(name);
	char *end = strchr(name, '.');
	if (end) {

		*end = '\0';
		list = (NodeList*) get(name)->val;

	}

	free(name);
	list->rem((void*) key);

}

NodeTree::Key* NodeTree::find(char *name, NodeList *list) {

	if (!list->len) return NULL;

	for (unsigned int i = 0; i < list->len; i++) {

		Key *key = (Key*) list->get(i);
		if (!strcmp(key->name, name)) return key;

	}

	return NULL;

}

void NodeTree::del(Key *key) {

	free(key->name);

	switch (key->t) {

		case hval: free(key->val);
		case val: delete key;
		return;

		case type::key:
		break;

	}

	NodeList *list = (NodeList*) key->val;
	delete key;

	for (unsigned int i = 0; i < list->len; i++) {

		key = (Key*) list->get(i);
		del(key);

	}

	delete list;

}
