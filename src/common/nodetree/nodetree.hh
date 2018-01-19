#ifndef GAME_COMMON_NODETREE

#include "nodelist.hh"

class NodeTree { // i  need mutexes!!

	public:

		typedef enum { key, val, hval } type;

		typedef struct {

			char *name;
			void *val;
			type t;

		} Key;

		NodeTree();
		~NodeTree();

		Key* get(char *name);
		void set(char *name, void *val, bool heap = false);
		void del(char *name); // me too

	private:

		Key *root;

		Key* find(char *name, NodeList *list);
		void del(Key *key); // consider reworking

};

#define GAME_COMMON_NODETREE
#endif
