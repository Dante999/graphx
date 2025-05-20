#ifndef TEMPLATE_GENERATOR_H
#define TEMPLATE_GENERATOR_H

#include "result.h"

Result create_tilemap_template(
	const char *path,
	int char_width, int char_height,
	int char_columns, int char_rows);

#endif // TEMPLATE_GENERATOR_H
