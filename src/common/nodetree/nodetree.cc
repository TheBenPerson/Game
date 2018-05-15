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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nodetree.hh"

NodeTree::NodeTree() {

	root = new Key;
	root->name = strdup("root");
	root->val = (intptr_t) new NodeList;
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

void NodeTree::set(char *str, intptr_t val, bool heap) { // todo: fix

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

				key->val = (intptr_t) new NodeList;
				key->t = type::key;

			} else key->t = type::val;

			list->add((intptr_t) key);

		} else token = strtok(NULL, ".");

		if (!token) break;

	}

	free(name);
	if (key->t == hval) free((void*) key->val);

	key->t = heap ? hval : type::val;
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
	list->rem((intptr_t) key);

}

NodeTree::Key* NodeTree::find(char *name, NodeList *list) {

	if (!list->size) return NULL;

	for (unsigned int i = 0; i < list->size; i++) {

		Key *key = (Key*) (*list)[i];
		if (!strcmp(key->name, name)) return key;

	}

	return NULL;

}

void NodeTree::del(Key *key) {

	free(key->name);

	switch (key->t) {

		case hval: free((void*) key->val);
		case val: delete key;
		return;

		case type::key:
		break;

	}

	NodeList *list = (NodeList*) key->val;
	delete key;

	for (unsigned int i = 0; i < list->size; i++) {

		key = (Key*) (*list)[i];
		del(key);

	}

	delete list;

}
