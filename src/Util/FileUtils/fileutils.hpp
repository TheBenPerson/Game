#ifndef HG_Util_FileUtils_H

namespace FileUtils {

	extern const char * ErrorInvalidPath;
	extern const char * ErrorIsDir;
	extern const char * ErrorPermDenied;
	extern const char * ErrorWriting;
	extern const char * Wrote;

	int LoadFile(const char * Path, char ** Buffer);
	bool WriteFile(const char * Path, char * Buffer);

};

#define HG_Util_FileUtils_H
#endif
