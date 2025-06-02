#ifndef UTIL_PATHS_H
#define UTIL_PATHS_H
/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stdbool.h>
#include <stddef.h>

void util_filename_from_path(const char *path, char *filename);
char *util_remove_file_extension(char *filename);


/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef UTIL_PATHS_IMPLEMENTATION

#include <string.h>
#include <ctype.h>
#include <assert.h>

void util_filename_from_path(const char *path, char *filename)
{
	assert(path != NULL);
	assert(filename != NULL);

	char *separator = strrchr(path, '/');

	if (separator == NULL) {
		*filename = '\0';
		return;
	}

	strcpy(filename, separator+1);
}

char *util_remove_file_extension(char *filename)
{
	assert(filename != NULL);

	char *separator = strrchr(filename, '.');

	if (separator != NULL) {
		*separator = '\0';
	}

	return filename;
}

#endif // UTIL_PATHS_IMPLEMENTATION

#endif // UTIL_PATHS_H

