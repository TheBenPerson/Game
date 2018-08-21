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

#ifndef GAME_CLIENT_CONFIG
#define GAME_CLIENT_CONFIG

#include <confuse.h>

#define BOOL(item, val) CFG_BOOL(item, (cfg_bool_t) val, CFGF_NONE)
#define INT(item, val) CFG_INT(item, val, CFGF_NONE)
#define INT_LIST(item, val) CFG_INT_LIST(item, val, CFGF_NONE)
#define FLOAT(item, val) CFG_FLOAT(item, val, CFGF_NONE)
#define STRING(item, val) CFG_STR(item, val, CFGF_NONE)
#define STRING_LIST(item, val) CFG_STR_LIST(item, val, CFGF_NONE)
#define FUNCTION(item, val) CFG_FUNC(item, val)
#define CUSTOM(item, val, callback) CFG_INT_LIST_CB(item, val, CFGF_NONE, (cfg_callback_t) callback)
#define END CFG_END()

class Config {

	public:

		typedef cfg_opt_t Option;

		Config(char *path, Option *options, bool *result = NULL);
		~Config();

		bool getBool(char *item, unsigned int idex = 0);
		int getInt(char *item, unsigned int idex = 0);
		float getFloat(char *item, unsigned int idex = 0);
		char* getStr(char *item, unsigned int idex = 0);

		unsigned int getSize(char *item);

		void setBool(char *item, bool val);
		void setInt(char *item, int val);
		void setFloat(char *item, float val);
		void setStr(char *item, char *val);

	private:

		cfg_t *cfg;
		char *path;

};

#endif
