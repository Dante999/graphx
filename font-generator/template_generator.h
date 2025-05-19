#ifndef TEMPLATE_GENERATOR_H
#define TEMPLATE_GENERATOR_H

#include "libcutils/result.h"


Result template_generator_create(
		const char *path, 
		int char_width, int char_height,
		int char_columns, int char_rows);

#endif // TEMPLATE_GENERATOR_H
