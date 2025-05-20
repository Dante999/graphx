#include <stdio.h>

#define RESULT_IMPLEMENTATION
#include "result.h"

#define ARG_PARSER_IMPLEMENTATION
#include "arg_parser.h"


#include "template_generator.h"


static int handle_generate_template(void)
{
	const int char_width   = args_geti("--char_width", NULL);
	const int char_height  = args_geti("--char_height", NULL);
	const int char_columns = args_geti("--char_columns", NULL);
	const int char_rows    = args_geti("--char_rows", NULL);

	char filename[255];
	snprintf(filename, sizeof(filename), "template_%dx%d.png", char_width, char_height);
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
}

static int handle_tilemap_to_font(void)
{

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
}

/**
 * TODO: Refactor to something like this
 *
 * ./font-generator create-template --output_file=<FILE> --char_width=<VALUE> ...
 * ./font-generator convert-template --input-file=<FILE> --output-file=<FILE> ..
 */
int main(int argc, char *argv[])
{
	args_add_flag("--help");
	args_add_flag("--generate_template");
	args_add_flag("--tilemap_to_font");

	args_add_argument("--input_file");
	args_add_argument("--char_width");
	args_add_argument("--char_height");
	args_add_argument_with_default("--char_rows", "6");
	args_add_argument_with_default("--char_columns", "16");

	Result res = args_parse(argc, argv);
	if (!res.success) {
		fprintf(stderr, "ERROR: unable to parse launch arguments: %s\n", res.msg);
		args_help(argv[0]);
		return 1;
	}

	if (args_get_flag("--help")) {
		args_help(argv[0]);
	}

	/*
	 * TODO: input_file must only be set for command tilemap_to_font, for
	 * generate_template it is irrelevant. This can not yet be handled by
	 * arg_parser lib.
	 */
//	res = args_verify();
//	if (!res.success) {
//		fprintf(stderr, "ERROR: verifying args: %s\n", res.msg);
//		args_help(argv[0]);
//		return 1;
//	}

	if (args_get_flag("--generate_template")) {
		return handle_generate_template();
	}

	if (args_get_flag("--tilemap_to_font")) {
		return handle_tilemap_to_font();
	}

	return 0;
}
