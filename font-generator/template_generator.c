#include "template_generator.h"

#include "colors.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image_write.h"

#include <stdint.h>


#define RGB_PIXEL_SIZE   3

Result template_generator_create(const char *path,
		int char_width, int char_height,
		int char_columns, int char_rows)
{
	if (char_width < 1 || char_height < 1) {
		return make_result(
			false,
			"char width and height must be greater than zero!");
	}

	const int image_width  = char_width * char_columns;
	const int image_height = char_height * char_rows;

	printf("Generating template %s with data:\n"
		"char_width=%d char_height=%d columns=%d rows=%d\n",
		path, char_width, char_height, char_columns, char_rows);

	uint8_t *png_data = (uint8_t*)malloc(image_width * image_height * RGB_PIXEL_SIZE);

	// Fill the image with a solid color (red)
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

	// Write the image to a file
	int write_result = stbi_write_png(path, image_width, image_height, RGB_PIXEL_SIZE,
		png_data, image_width * RGB_PIXEL_SIZE);

	// Clean up
	free(png_data);

	if (write_result ==  0) {
		return make_result(false, "Failed to write png!\n");
	}
	else {
		return make_result_success();
	}
}

