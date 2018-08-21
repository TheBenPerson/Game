#ifndef GAME_COMMON_STRING
#define GAME_COMMON_STRING

#include <string.h>

#define STRING_CAT2(name, a, b) \
char name[strlen(a) + strlen(b) + 1]; \
strcpy(name, a); \
strcpy(name + strlen(a), b)

#define STRING_CAT3(name, a, b, c) \
char name[strlen(a) + strlen(b) + strlen(c) + 1]; \
strcpy(name, a); \
strcpy(name + strlen(a), b); \
strcpy(name + strlen(a) + strlen(b), c)

#define STRING_CAT4(name, a, b, c, d) \
char name[strlen(a) + strlen(b) + strlen(c) + strlen(d) + 1]; \
strcpy(name, a); \
strcpy(name + strlen(a), b); \
strcpy(name + strlen(a) + strlen(b), c); \
strcpy(name + strlen(a) + strlen(b) + strlen(c), d)

/*namespace String {

	char* cat(char *a, char *b);

}*/

#endif
