#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fileutil.h"

// Return 1 if path is a regular file, 0 otherwise.
int is_file (const char *path) {
	struct stat buf;
	int err;

	err = stat(path, &buf);
	if (-1 == err) {
		return 0;
	}

	return S_ISREG(buf.st_mode);
}

// Return 1 if path is a directory, 0 otherwise.
int is_dir (const char *path) {
	struct stat buf;
	int err;

	err = stat(path, &buf);
	if (-1 == err) {
		return 0;
	}

	return S_ISDIR(buf.st_mode);
}

