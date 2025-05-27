#include "atlas_generator.h"

#include "colors.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image_write.h"

#include <stdint.h>



Result create_tilemap_template(const char *path,
	int char_width, int char_height,
	int char_columns, int char_rows, int border_width)
{
	if (char_width < 1 || char_height < 1) {
		return result_make(
			false,
			"char width and height must be greater than zero!");
	}

	if (char_columns < 1 || char_rows < 1) {
		return result_make(
			false,
			"char columns and rows must be greater than zero!");

	}

	if (border_width < 1) {
		return result_make(
			false,
			"border width must be greater than zero!");

	}

	uint8_t rgb_border[RGB_PIXEL_SIZE]     = RGB_BORDER;
	uint8_t rgb_background[RGB_PIXEL_SIZE] = RGB_BACKGROUND;

	const int image_width  = (char_width  + border_width) * char_columns + border_width;
	const int image_height = (char_height + border_width) * char_rows    + border_width;

	printf("Generating template %s with data:\n"
		"char_width=%d char_height=%d columns=%d rows=%d\n",
		path, char_width, char_height, char_columns, char_rows);

	uint8_t *png_data = (uint8_t*)malloc(image_width * image_height * RGB_PIXEL_SIZE);



#if 1
	for (int y = 0; y < image_height; y++) {

		bool y_border = (y % (char_height+border_width)) == 0;

		for (int x = 0; x < image_width; x++) {
			int index = (y * image_width + x) * RGB_PIXEL_SIZE;

			bool x_border= (x % (char_width+border_width)) == 0;

			if (x_border || y_border) {
				png_data[index + 0] = rgb_border[0];
				png_data[index + 1] = rgb_border[1];
				png_data[index + 2] = rgb_border[2];
			}
			else {
				png_data[index + 0] = rgb_background[0];
				png_data[index + 1] = rgb_background[1];
				png_data[index + 2] = rgb_background[2];
			}
		}
	}

#else
for (int y = 0; y < image_height; y++) {

	bool alternate_row = (y / char_height) % 2;

	for (int x = 0; x < image_width; x++) {
		int index = (y * image_width + x) * RGB_PIXEL_SIZE;

		bool alternate_column = (x / char_width) % 2;

		if (alternate_column ^ alternate_row) {
			png_data[index + 0] = rgb_tile_a[0];
			png_data[index + 1] = rgb_tile_a[1];
			png_data[index + 2] = rgb_tile_a[2];
		}
		else {
			png_data[index + 0] = rgb_tile_b[0];
			png_data[index + 1] = rgb_tile_b[1];
			png_data[index + 2] = rgb_tile_b[2];
		}
	}
}
#endif
	// Write the image to a file
	int write_result = stbi_write_png(path, image_width, image_height, RGB_PIXEL_SIZE,
		png_data, image_width * RGB_PIXEL_SIZE);

	// Clean up
	free(png_data);

	if (write_result ==  0) {
		return result_make(false, "Failed to write png!\n");
	}
	else {
		return result_make_success();
	}
}

