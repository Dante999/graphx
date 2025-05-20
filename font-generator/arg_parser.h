#ifndef ARG_PARSER_H
#define ARG_PARSER_H

/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stdint.h>

#include "result.h"

// TODO: void args_add_command(const char *cmd, const char *description);
void args_add_flag(const char *key);
void args_add_argument(const char *key);
void args_add_argument_with_default(const char *key, const char *default_value);

Result args_parse(int argc, char *argv[]);
Result args_verify(void);
void args_print(void);
void args_help(const char *program_name);

const char *args_gets(const char *key, Result *result);
int         args_geti(const char *key, Result *result);
bool        args_get_flag(const char *key);
bool        args_is_argument_set(const char *key);


/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef ARG_PARSER_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_ARGS  100
#define MAX_FLAGS 100

#define MAX_KEY_LEN        255
#define MAX_VALUE_LEN      255


typedef struct {
	char key[MAX_KEY_LEN];
	char default_value[MAX_VALUE_LEN];
	char value[MAX_VALUE_LEN];
} Arg;

typedef struct {
	size_t count;
	Arg entries[MAX_ARGS];
} Args;

typedef struct {
	char key[MAX_KEY_LEN];
	bool value;
} Flag;

typedef struct {
	size_t count;
	Flag entries[MAX_FLAGS];
} Flags;



static Args  g_args;
static Flags g_flags;


static bool args_set_flag_value(const char *val)
{
	for (size_t i=0; i < g_flags.count; ++i) {
		if (strncmp(g_flags.entries[i].key, val, MAX_KEY_LEN) == 0) {
			g_flags.entries[i].value = true;
			return true;
		}
	}

	return false;
}

static bool args_set_arg_value(const char *val)
{
	char *delimiter = strchr(val, '=');

	if (delimiter == NULL) {
		return false;
	}

	for (size_t i=0; i < g_args.count; ++i) {

		Arg *stored_arg = &g_args.entries[i];

		if (strncmp(stored_arg->key, val, delimiter-val) != 0) {
			continue;
		}

		if (strlen(delimiter) < 1) {
			return false;
		}

		strncpy(stored_arg->value, (delimiter+1), MAX_KEY_LEN);
		return true;
	}

	return false;
}


static Arg *args_get_arg_ptr(const char *key)
{
	for (size_t i = 0; i < g_args.count; ++i) {
		Arg *arg= &g_args.entries[i];

		if (strncmp(arg->key, key, MAX_KEY_LEN) == 0) {
			return arg;
		}
	}

	return NULL;
}

void args_add_argument_with_default(const char *key, const char *default_value)
{
	if (g_args.count < MAX_ARGS) {
		Arg *arg = &g_args.entries[g_args.count++];
		strncpy(arg->key, key, MAX_KEY_LEN);
		arg->value[0] = '\0';
		strncpy(arg->default_value, default_value, MAX_VALUE_LEN);
	}
}

void args_add_argument(const char *key) {
	args_add_argument_with_default(key, "");
}

void args_add_flag(const char* key)
{
	if (g_flags.count < MAX_FLAGS) {
		Flag *flag = &g_flags.entries[g_flags.count++];
		strncpy(flag->key, key, MAX_KEY_LEN);
		flag->value = false;
	}
}

void args_help(const char *program_name)
{
	printf("Usage: %s [Flags] [Arguments]\n", program_name);
	printf("\n");
	printf("Flags\n");
	for (size_t i=0; i < g_flags.count; ++i) {
		Flag *flag= &g_flags.entries[i];

		printf("\t%s\n", flag->key);
	}

	printf("\n");
	printf("Options\n");
	for (size_t i=0; i < g_args.count; ++i) {
		Arg *arg = &g_args.entries[i];

		char tmp[512];
		snprintf(tmp, sizeof(tmp), "%s=<VALUE>", arg->key);
		printf("\t%-20s [default: %s]\n", tmp, arg->default_value);
	}
}

void args_print(void)
{
	printf("###########################################\n");
	printf("# CURRENT ARGUMENT VALUES                 #\n");
	printf("###########################################\n");

	for (size_t i = 0; i < g_args.count; ++i) {
		Arg *arg = &g_args.entries[i];

		printf("\t%s=%s\n", arg->key,
				strlen(arg->value) > 0 ? arg->value : arg->default_value);
	}

	printf("###########################################\n");
}

Result args_parse(int argc, char *argv[])
{

	for (int i=1; i < argc; ++i) {
		const char *val = argv[i];

		if (args_set_flag_value(val))  continue;
		if (args_set_arg_value(val)) continue;

		return result_make(false, "undefined argument: %s", val);
	}

	return result_make_success();
}

Result args_verify(void) {

	for (size_t j=0; j < g_args.count; ++j) {
		Arg *stored_arg = &g_args.entries[j];

		if (strlen(stored_arg->default_value) == 0 &&
			strlen(stored_arg->value) == 0) {
			return result_make(
					false,
					"At least one mandatory argument is not set: %s",
					stored_arg->key);
		}
	}
	return result_make_success();
}

bool args_is_argument_set(const char *key)
{
	Arg *arg = args_get_arg_ptr(key);

	if (arg == NULL)             return false;
	if (strlen(arg->value) == 0) return false;

	return true;
}

const char *args_gets(const char *key, Result *result)
{
	Arg *arg= args_get_arg_ptr(key);

	if (arg == NULL) {
		result_set(result, false, "given key '%s' does not exist", key);
		return NULL;
	}
	else if (strlen(arg->value) >0) {
		result_set(result, true, "giving overriden value");
		return arg->value;
	}
	else {
		result_set(result, true, "giving default value");
		return arg->default_value;
	}
}

int args_geti(const char *key, Result *result)
{
	return atoi(args_gets(key, result));
}

bool args_get_flag(const char *key)
{
	for (size_t i=0; i < g_flags.count; ++i) {
		Flag *flag= &g_flags.entries[i];

		if (strncmp(flag->key, key, MAX_KEY_LEN) == 0) {
			return flag->value;
		}
	}

	return false;
}


#endif // ARG_PARSER_IMPLEMENTATION


#endif // ARG_PARSER_H
