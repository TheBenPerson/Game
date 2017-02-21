#ifndef HG_Util_FileUtils_H

#include <stddef.h>

size_t loadFile(char *path, char *buffer[]);
bool writeFile(char *path, char *buffer);

#define HG_Util_FileUtils_H
#endif
