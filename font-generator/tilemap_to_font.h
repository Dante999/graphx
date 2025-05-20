#ifndef TILEMAP_TO_FONT_H
#define TILEMAP_TO_FONT_H

#include "libcutils/result.h"

Result tilemap_to_font(
	const char *path, 
	int char_width, int char_height,
	int char_columns, int char_rows);

#endif // TILEMAP_TO_FONT_H
