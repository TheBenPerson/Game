#include <stdio.h>

void cputs(char *string, char *escape, bool bold) {

	const char *mod = bold ? "\e[1m" : "";
	printf("%s%s%s%s\n", escape, mod, string, "\e[m");

}
