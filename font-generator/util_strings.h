#ifndef UTIL_STRINGS_H
#define UTIL_STRINGS_H
/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stdbool.h>
#include <stddef.h>

bool  util_is_whitespace(char c);
bool  util_strstartswith(const char *complete_str, const char *starter);
bool  util_stranyof(const char *s, const char **probe_values, size_t probe_count);
char *util_strltrim(char *s);
char *util_strrtrim(char *s);
char *util_strtrim(char *s);



/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef UTIL_STRINGS_IMPLEMENTATION

#include <string.h>

bool util_is_whitespace(char c)
{
	return (strchr(" \t\r\n", c) != NULL) ? true : false;
}

char *util_strltrim(char *s)
{
	const size_t len = strlen(s);

	if (len == 0) {
		return s;
	}

	if (!util_is_whitespace(*s)) {
		return s;
	}

	size_t idx_src = 0;
	size_t idx_dst = 0;

	while (util_is_whitespace(s[idx_src]) && idx_src < len) {
		++idx_src;
	}

	while (idx_src < len) {
		s[idx_dst++] = s[idx_src++];
	}

	s[idx_dst] = '\0';
	return s;
}

char *util_strrtrim(char *s)
{

	size_t idx_end = strlen(s);

	if (idx_end == 0) {
		return s;
	}

	--idx_end;

	while (util_is_whitespace(s[idx_end]) && idx_end > 0) {
		s[idx_end--] = '\0';
	}

	return s;
}

char *util_strtrim(char *s)
{
	util_strrtrim(s);
	return util_strltrim(s);
}

bool util_strstartswith(const char *complete_str, const char *starter)
{
	return (strncmp(complete_str, starter, strlen(starter)) == 0) ? true
	                                                              : false;
}

bool util_stranyof(const char *s, const char **probe_values, size_t probe_count)
{
	for (size_t i=0; i < probe_count; ++i){
		if (strcmp(s, probe_values[i]) == 0) {
			return true;
		}
	}

	return false;
}


#endif // UTIL_STRINGS_IMPLEMENTATION

#endif // UTIL_STRINGS_H

