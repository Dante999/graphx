#ifndef ARG_PARSER_H
#define ARG_PARSER_H

/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stdint.h>

#include "result.h"

void args_add_command(const char *name, const char *description);
void args_add_flag(const char *name);
void args_add_option(const char *name);
void args_add_option_with_default(const char *name, const char *default_value);

Result args_parse(int argc, char *argv[]);

void args_print_values(void);
void args_print_help(const char *program_name);

const char *args_get_option_as_str(const char *name, Result *result);
int         args_get_option_as_int(const char *name, Result *result);

bool args_is_command_given(const char *name);
bool args_is_flag_given(const char *name);

/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef ARG_PARSER_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_OPTIONS     100
#define MAX_FLAGS    100
#define MAX_COMMANDS 100

#define MAX_KEY_LEN    255
#define MAX_VALUE_LEN  255
#define MAX_DESC_LEN   255


typedef struct {
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];
	bool has_default;
	char default_value[MAX_VALUE_LEN];
	bool is_given;
} Option;

typedef struct {
	char key[MAX_KEY_LEN];
	bool is_given;
} Flag;

typedef struct {
	char key[MAX_KEY_LEN];
	char description[MAX_DESC_LEN];
	bool is_given;
} Command;

// ----------------

typedef struct {
	size_t count;
	Option entries[MAX_OPTIONS];
} Options;


typedef struct {
	size_t count;
	Flag entries[MAX_FLAGS];
} Flags;


typedef struct {
	size_t count;
	Command entries[MAX_COMMANDS];
} Commands;

// ----------------

static Options  g_options  = {0};
static Flags    g_flags    = {0};
static Commands g_commands = {0};


static bool args_parse_as_flag(const char *text)
{
	for (size_t i=0; i < g_flags.count; ++i) {
		Flag *it = &g_flags.entries[i];

		if (strncmp(it->key, text, MAX_KEY_LEN) == 0) {
			it->is_given = true;
			return true;
		}
	}

	return false;
}

static bool args_parse_as_command(const char *text)
{
	for (size_t i=0; i < g_commands.count; ++i) {
		Command *it = &g_commands.entries[i];

		if (strncmp(it->key, text, MAX_KEY_LEN) == 0) {
			it->is_given = true;
			return true;
		}
	}

	return false;
}

static bool args_parse_as_option(const char *text)
{
	char *delimiter = strchr(text, '=');

	if (delimiter == NULL) {
		return false;
	}

	for (size_t i=0; i < g_options.count; ++i) {

		Option *it = &g_options.entries[i];

		if (strncmp(it->key, text, delimiter-text) != 0) {
			continue;
		}

		if (strlen(delimiter) < 1) {
			return false;
		}

		strncpy(it->value, (delimiter+1), MAX_KEY_LEN);
		it->is_given = true;
		return true;
	}

	return false;
}

static Command* args_find_command(const char *name)
{
	for (size_t i = 0; i < g_commands.count; ++i) {
		Command *it = &g_commands.entries[i];

		if (strncmp(it->key, name, MAX_KEY_LEN) == 0) {
			return it;
		}
	}

	return NULL;
}

static Option *args_find_option(const char *name)
{
	for (size_t i = 0; i < g_options.count; ++i) {
		Option *it = &g_options.entries[i];

		if (strncmp(it->key, name, MAX_KEY_LEN) == 0) {
			return it;
		}
	}

	return NULL;
}

static Flag* args_find_flag(const char *name)
{
	for (size_t i = 0; i < g_flags.count; ++i) {
		Flag *it = &g_flags.entries[i];

		if (strncmp(it->key, name, MAX_KEY_LEN) == 0) {
			return it;
		}
	}

	return NULL;
}

void args_add_command(const char *name, const char *description)
{
	if (g_commands.count < MAX_COMMANDS) {
		Command *it = &g_commands.entries[g_commands.count++];

		strncpy(it->key, name, MAX_KEY_LEN);
		strncpy(it->description, description, MAX_DESC_LEN);
		it->is_given = false;
	}
}

void args_add_option(const char *name) {
	if (g_options.count < MAX_OPTIONS) {
		Option *it = &g_options.entries[g_options.count++];

		strncpy(it->key, name, MAX_KEY_LEN);

		it->is_given         = false;
		it->value[0]         = '\0';

		it->has_default      = false;
		it->default_value[0] = '\0';
	}
}

void args_add_option_with_default(const char *name, const char *default_value)
{
	if (g_options.count < MAX_OPTIONS) {
		Option *it = &g_options.entries[g_options.count++];

		strncpy(it->key, name, MAX_KEY_LEN);

		it->is_given         = false;
		it->value[0]         = '\0';

		it->has_default      = true;
		strncpy(it->default_value, default_value, MAX_VALUE_LEN);
	}
}


void args_add_flag(const char* name)
{
	if (g_flags.count < MAX_FLAGS) {
		Flag *it = &g_flags.entries[g_flags.count++];

		strncpy(it->key, name, MAX_KEY_LEN);
		it->is_given = false;
	}
}

void args_print_help(const char *program_name)
{
	printf("Usage: %s Command [Flags] [Options]\n", program_name);

	printf("\n");
	printf("Commands\n");
	for (size_t i=0; i < g_commands.count; ++i) {
		Command *it = &g_commands.entries[i];

		printf("\t%-25s %s\n", it->key, it->description);
	}

	printf("\n");
	printf("Flags\n");
	for (size_t i=0; i < g_flags.count; ++i) {
		Flag *it = &g_flags.entries[i];

		printf("\t%s\n", it->key);
	}

	printf("\n");
	printf("Options\n");
	for (size_t i=0; i < g_options.count; ++i) {
		Option *it = &g_options.entries[i];

		char tmp[512];
		snprintf(tmp, sizeof(tmp), "%s=<VALUE>", it->key);
		printf("\t%-20s [default: %s]\n", tmp, it->default_value);
	}
}

void args_print_values(void)
{
	printf("###########################################\n");
	printf("# CURRENT ARGUMENT VALUES                 #\n");
	printf("###########################################\n");

	for (size_t i = 0; i < g_options.count; ++i) {
		Option *arg = &g_options.entries[i];

		printf("\t%s=%s\n", arg->key,
				strlen(arg->value) > 0 ? arg->value : arg->default_value);
	}

	printf("###########################################\n");
}

Result args_parse(int argc, char *argv[])
{
	// argv[0] is the program name, so skip it
	for (int i=1; i < argc; ++i) {
		const char *val = argv[i];

		if (args_parse_as_command(val)) continue;
		if (args_parse_as_flag(val))    continue;
		if (args_parse_as_option(val))  continue;

		return result_make(false, "unknown argument given: '%s'", val);
	}

	return result_make_success();
}

bool args_is_option_given(const char *name)
{
	Option *it = args_find_option(name);

	if (it == NULL)             return false;
	if (strlen(it->value) == 0) return false;

	return true;
}

const char *args_get_option_as_str(const char *name, Result *result)
{
	Option *it = args_find_option(name);

	if (it == NULL) {
		result_set(result, false, "option '%s' is not defined!", name);
		return NULL;
	}
	else if (!it->is_given && !it->has_default) {
		result_set(result, false, "option '%s' is not set and does not have a default value!", name);
		return NULL;
	}
	else if (it->is_given) {
		result_set(result, true, "returning given value");
		return it->value;
	}
	else {
		result_set(result, true, "returning default value");
		return it->default_value;
	}
}

int args_get_option_as_int(const char *key, Result *result)
{
	const char *s = (args_get_option_as_str(key, result));
	if (s != NULL) {
		return atoi(s);
	}
	else {
		return INT_MIN;
	}
}

bool args_is_flag_given(const char *name)
{
	Flag *it = args_find_flag(name);

	if (it == NULL) return false;
	else            return it->is_given;
}

bool args_is_command_given(const char *name)
{
	Command *it = args_find_command(name);

	if (it == NULL) return false;
	else            return it->is_given;
}

#endif // ARG_PARSER_IMPLEMENTATION


#endif // ARG_PARSER_H
