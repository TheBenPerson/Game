#ifndef HG_Util_Config_H

#include "Util/NodeList/nodelist.hpp"

class Config {

	public:

		struct Setting {

			char *name;
			void *val;

			bool freeName;
			bool freeVal;
			bool isString;

		};

		void add(char *name, void *val);
		Setting* get(char *name);
		bool load(char *path);
		void save(char* path);
		void set(char *name, void *val);

		~Config();

	private:

		NodeList settings;

};

#define HG_Util_Config_H
#endif
