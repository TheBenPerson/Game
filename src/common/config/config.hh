#ifndef GAME_CLIENT_CONFIG

#include "nodetree.hh"

class Config: public NodeTree {

	public:

		Config(char *dir);

		void load(char *path);

	private:

		char *dir;

};

#define GAME_CLIENT_CONFIG
#endif
