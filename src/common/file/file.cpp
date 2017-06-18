/*

Game Development Build
https://github.com/TheBenPerson/Game

Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "file.hpp"

ssize_t loadFile(char *path, char *buffer[]) {

	int fd = open(path, O_RDONLY);
	if (fd == -1) {

		fprintf(stderr, "Error opening file '%s': %s\n", path, strerror(errno));
		return -1;

	}

	struct stat info;
	fstat(fd, &info);

	size_t size = info.st_size;
	*buffer = (char*) malloc(size);

	ssize_t result = read(fd, *buffer, size);

	if (result == -1) {

		fprintf(stderr, "Error reading file '%s': %s\n", path, strerror(errno));

		close(fd);
		return -1;

	}

	close(fd);
	return result;

}

bool writeFile(char *path, char *buffer, ssize_t len) {

	int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0x1A4);
	if (fd == -1) {

		fprintf(stderr, "Error opening file '%s': %s\n", path, strerror(errno));
		return false;

	}

	ssize_t result = write(fd, buffer, len);

	if (result == -1) {

		fprintf(stderr, "Error writing file '%s': %s\n", path, strerror(errno));

		close(fd);
		return false;

	}


	close(fd);
	return true;

}
