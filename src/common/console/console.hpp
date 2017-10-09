#ifndef GAME_COMMON_CONSOLE

#define RED		"\e[31m"
#define GREEN	"\e[32m"
#define YELLOW	"\e[33m"
#define BLUE	"\e[34m"
#define MAGENTA	"\e[35m"
#define CYAN	"\e[36m"
#define WHITE	"\e[36m"

extern void cputs(char *string, char *escape = GREEN, bool bold = true);

#define GAME_COMMON_CONSOLE
#endif
