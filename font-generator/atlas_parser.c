#include "atlas_parser.h"

#include "colors.h"

#include <assert.h>
#include <stdint.h>
#include <strings.h>
#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image.h"

static Result write_font_file(
		const char *output_file,
		int char_width,
		int char_height,
		const uint8_t *data,
		size_t len);

Result atlas_parser(
	const char *input_file,
	const char *output_file,
	int border_width,
	int char_width,
	int char_height)
{
	if (char_width < 1 || char_height < 1) {
		return result_make(
			false,
			"char width and height must be greater than zero!");
	}

	int image_width  = 0;
	int image_height = 0;
	int channels = 0;

	uint8_t *png_data = stbi_load(input_file, &image_width, &image_height, &channels, 0);

	if (channels != RGB_PIXEL_SIZE) {
		return result_make(
				false,
				"expected png color channels to be %d but is %d",
				RGB_PIXEL_SIZE, channels);
	}

	const int char_columns = (image_width  - border_width) / (char_width  + border_width);
	const int char_rows    = (image_height - border_width) / (char_height + border_width);
	const int char_width_bytes  = char_width;
	const int char_height_bytes = 1; // TODO: Fixme, works only with a max char_width of 8 pixels

	printf("DEBUG: parsing atlas, calculated char_columns=%d char_rows=%d\n", char_columns, char_rows);
	const size_t array_size = char_columns * char_rows * char_height_bytes * char_width_bytes;
	assert(array_size > 0);
	printf("DEBUG: array_size=%zu\n", array_size);

	uint8_t *array_data = (uint8_t*) malloc(array_size);

	if (array_data == NULL) {
		return result_make_error(errno);
	}
	memset(array_data, 0, array_size);


	uint8_t rgb_black[RGB_PIXEL_SIZE] = RGB_BLACK;

	for (int y = 0; y < image_height; y++) {
		bool y_border = (y % (char_height+border_width)) == 0;


		for (int x = 0; x < image_width; x++) {

			bool x_border= (x % (char_width+border_width)) == 0;

			if (x_border || y_border) continue;

			int png_index = (y * image_width + x) * RGB_PIXEL_SIZE;

			if (COLOR_EQUALS(&png_data[png_index], &rgb_black[0])) {

#if 0
				const int cell_x = x / (char_width+border_width*2);
				const int cell_y = y / (char_height+border_width*2);
				printf("DEBUG: x=%d cell_x=%d\n", x, cell_x);
				// TODO: compensate border width
#else
				const int x_correction = border_width + (x / (char_width+border_width))  * border_width;
				const int y_correction = border_width + (y / (char_height+border_width)) * border_width;
				const int x_array = x - x_correction;
				const int y_array = y - y_correction;
				const int bytes_per_row = char_width * char_columns;
				int index_array = (x_array) + ((y_array+1) / 8) * bytes_per_row;
				//                                      ^
				//TODO: font_height is 7, therefore jumping to
				//the next "line" in the byte array is one step
				//to late
#endif
				int y_bit = y_array % 8;
				assert((size_t) index_array < array_size);
				printf("x=%03d y=%03d | x_cor=%03d y_cor=%03d | array_index=%d array_bit=%d\n", x, y, x_correction, y_correction, index_array, y_bit);
				array_data[index_array] |= (1u << y_bit);
			}
		}

		if (y > 10) break;
	}

	stbi_image_free(png_data);

	return write_font_file(output_file, char_width, char_height, array_data, array_size);
}

static Result write_font_file(
		const char *output_file,
		int char_width,
		int char_height,
		const uint8_t *data,
		size_t len)
{

	FILE *f = fopen(output_file, "w");

	if (f == NULL) {
		return result_make_error(errno);
	}

	// TODO: filename only, toupper, replace any non letter with underscore
	char header_guard[255];
	strcpy(header_guard, "FONT_5x7_TEST_H");
	// TODO: filename only, tolower, remove file extension
	char variable_name[255];
	strcpy(variable_name, "font5x7_test");



	fprintf(f, "#ifndef %s\n", header_guard);
	fprintf(f, "#define %s\n", header_guard);
	fprintf(f, "\n");
	fprintf(f, "#include <stdint.h>\n");
	fprintf(f, "static const uint8_t %s[] = {\n", variable_name);

	fprintf(f, "\t%d, // font width\n", char_width);
	fprintf(f, "\t%d, // font height\n", char_height);

	for (size_t i=0; i < len; ++i) {
		if ((i % char_width) == 0 && (i != 0)) {
			fprintf(f, "\n\t");
		}
		else if (i == 0) {
			fprintf(f, "\t");
		}

		fprintf(f, "0x%02x, ", (int) data[i]);

	}

	fprintf(f, "};\n");
	fprintf(f, "\n");
	fprintf(f, "#endif // %s\n", header_guard);

	if (fclose(f) != 0) {
		return result_make_error(errno);
	}

	return result_make_success();

}


