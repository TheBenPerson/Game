#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "FileUtils.hpp"

int FileUtils::LoadFile(const char * Path, char ** Buffer) {

	int Error = 0;
	unsigned int Size = 0;

	int FileDescriptor = open(Path, O_RDONLY);

	if (FileDescriptor == -1)
		Error = errno;

	if (!Error) {

		struct stat FileInfo;

		fstat(FileDescriptor, &FileInfo);

		Size = FileInfo.st_size;

		*Buffer = new char[Size];

		if (read(FileDescriptor, *Buffer, Size) == -1)
			Error = errno;

		close(FileDescriptor);

	}

	if (Error) {

		fprintf(stderr, "Error loading file '%s': %s.\n", Path, strerror(errno));

	} else {

		printf("Loaded file '%s'.\n", Path);

	}

	return Size;

}

bool FileUtils::WriteFile(const char * Path, char * Buffer) {

	bool Return = false;

	unsigned int FinalMsgLen = 0;

	char * FinalMsg = NULL;

	int FileDescriptor = open(Path, O_CREAT | O_TRUNC | O_WRONLY, 0x1A4);

	int error = errno;

	if (!error) {

		write(FileDescriptor, Buffer, strlen(Buffer));

		error = errno;

		close(FileDescriptor);

	}

	if (error) {

		fprintf(stderr, "Error writing file '%s': %s.\n", Path, strerror(errno));

	} else {

		printf("Wrote file '%s'.\n", Path);

		Return = true;

	}

	delete[] FinalMsg;

	return Return;

}
