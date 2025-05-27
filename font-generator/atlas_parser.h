#ifndef ATLAS_PARSER_H
#define ATLAS_PARSER_H

#include "result.h"

Result atlas_parser(
	const char *input_file,
	const char *output_file,
	int border_width,
	int char_width,
	int char_height);

#endif // ATLAS_PARSER_H
