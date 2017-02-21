#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "file.hpp"

size_t loadFile(char *path, char *buffer[]) {

	int fd = open(path, O_RDONLY);
	if (fd == -1) {

		fprintf(stderr, "Error opening file '%s': %s.\n", path, strerror(errno));
		return NULL;

	}

	struct stat info;
	fstat(fd, &info);

	size_t size = info.st_size;
	*buffer = (char*) malloc(size);

	ssize_t result = read(fd, *buffer, size);

	if (result == -1) {

		fprintf(stderr, "Error reading file '%s': %s.\n", path, strerror(errno));

		close(fd);
		return -1;

	}

	close(fd);
	return result;

}

bool writeFile(char *path, char *buffer, ssize_t len) {

	int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0x1A4);
	if (fd == -1) {

		fprintf(stderr, "Error opening file '%s': %s.\n", path, strerror(errno));
		return false;

	}

	ssize_t result = write(fd, buffer, len);

	if (result == -1) {

		fprintf(stderr, "Error writing file '%s': %s.\n", path, strerror(errno));

		close(fd);
		return false;

	}


	close(fd);
	return true;

}
