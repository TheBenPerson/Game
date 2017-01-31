#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util/Config/Config.hpp"
#include "Util/FileUtils/FileUtils.hpp"

Config * Config::LoadConfig(const char * Path) {

	char * Buffer = NULL;

	unsigned int Size = FileUtils::LoadFile(Path, &Buffer);

	Config * tConfig = NULL;

	if (Size != -1) {

		tConfig = new Config();

		tConfig->Values = new Value();

		Value * tValue = tConfig->Values;

		char * pStart = Buffer;
		char * pStop = Buffer;

		bool Commented = false;
		bool BlockStyle = false;

		for (unsigned int i = 0; i < Size; i++) {

			char tChar = Buffer[i];

			switch (tChar) {

				case '/': {

					char tChar2 = Buffer[i + 1];

					if (tChar2 == '/') {

						i++;

						while ((++i < Size) && (Buffer[i] != '\n')) {}

						break;

					} else if (tChar2 == '*') {

						i++;

						while ((++i < Size - 1) && (Buffer[i] != '*') && (Buffer[i + 1] != '/')) {}

						break;

					}

				}

				default:

					printf("%c\n", tChar);

			}

		}

		return tConfig;

	}

}

void Config::WriteConfig(Config * PConfig) {



}

Config::Config() {}

Config::~Config() {}

Config::Value * Config::Value::GetLast() {

	struct Value * tValue = this;

	while (tValue->NextValue != NULL)
		tValue = tValue->NextValue;

	return tValue;

}
