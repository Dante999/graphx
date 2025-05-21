#include <stdio.h>

#define RESULT_IMPLEMENTATION
#include "result.h"

#define ARG_PARSER_IMPLEMENTATION
#include "arg_parser.h"


#include "atlas_generator.h"

static void print_and_increase_on_error(const Result *result, int *error_counter)
{
	if (!result->success) {
		printf("ERROR: %s\n", result->msg);
		(*error_counter)++;
	}
}

static int handle_generate_template(void)
{
	int error_counter = 0;
	Result res;

	const char *output_file = args_get_option_as_str("--output_file", &res);
	print_and_increase_on_error(&res, &error_counter);

	const int char_width = args_get_option_as_int("--char_width", &res);
	print_and_increase_on_error(&res, &error_counter);
	
	const int char_height = args_get_option_as_int("--char_height", &res);
	print_and_increase_on_error(&res, &error_counter);
	
	const int char_columns = args_get_option_as_int("--char_columns", &res);
	print_and_increase_on_error(&res, &error_counter);
	
	const int char_rows = args_get_option_as_int("--char_rows", &res);
	print_and_increase_on_error(&res, &error_counter);

	if (error_counter > 0) {
		return 1;
	}

	Result gen_result = create_tilemap_template(
			output_file,
			char_width, char_height,
			char_columns, char_rows);

	if (gen_result.success) {
		printf("INFO: Successfully created template file!\n");
		return 0;
	}
	else {
		printf("ERROR: Failed to create template file: %s\n", gen_result.msg);
		return 1;
	}
}

static int handle_parse_atlas(void)
{
#if 0
	const int char_width   = args_geti("--char_width", NULL);
	const int char_height  = args_geti("--char_height", NULL);
	const int char_columns = args_geti("--char_columns", NULL);
	const int char_rows    = args_geti("--char_rows", NULL);

	char filename[255];
	snprintf(filename, sizeof(filename), "template_%dx%d.h", char_width, char_height);
	Result gen_result = create_tilemap_template(
			filename,
			char_width, char_height,
			char_columns, char_rows);

	if (gen_result.success) {
		printf("INFO: Successfully created template file!\n");
		return 0;
	}
	else {
		printf("ERROR: Failed to create template file: %s\n", gen_result.msg);
		return 1;
	}
#endif

	return 0;
}


int main(int argc, char *argv[])
{
	args_add_command("create_atlas", 
			"creates an empty font atlas with the given dimensions");

	args_add_command("parse_atlas", 
			"converts a given font atlas to a c header file");

	args_add_flag("--help");

	args_add_option("--output_file");
	args_add_option("--char_width");
	args_add_option("--char_height");
	args_add_option_with_default("--char_rows", "6");
	args_add_option_with_default("--char_columns", "16");

	Result res = args_parse(argc, argv);
	if (!res.success) {
		fprintf(stderr, "ERROR: unable to parse launch arguments: %s\n", res.msg);
		args_print_help(argv[0]);
		return 1;
	}

	args_print_values();

	if (args_is_flag_given("--help")) {
		args_print_help(argv[0]);
	}

	if (args_is_command_given("create_atlas")) {
		printf("INFO: executing command 'create_atlas'\n"); 
		return handle_generate_template();
	}

	if (args_is_command_given("parse_atlas")) {
		printf("INFO: executing command 'parse_atlas'\n"); 
		return handle_parse_atlas();
	}

	printf("ERROR: no command given!\n");
	args_print_help(argv[0]);
	return 0;
}
