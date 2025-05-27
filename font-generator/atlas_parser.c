#include "atlas_parser.h"

#include "colors.h"

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

	const int char_columns = (image_width  - border_width) / (char_width   +border_width);
	const int char_rows    = (image_height - border_width) / (char_height  +border_width);

	// TODO: Fixme, works only with a max char_width of 8 pixels
	const size_t array_size = char_columns * char_rows * char_width;

	uint8_t *array_data = (uint8_t*) malloc(array_size);
	memset(array_data, 0, array_size);


	uint8_t rgb_black[RGB_PIXEL_SIZE] = RGB_BLACK;

	for (int y = 0; y < image_height; y++) {
		bool y_border = (y % (char_height+border_width)) == 0;


		for (int x = 0; x < image_width; x++) {

			bool x_border= (x % (char_width+border_width)) == 0;

			if (x_border || y_border) {
				continue;
			}

			int index_png = (y * image_width + x) * RGB_PIXEL_SIZE;

			if (png_data[index_png + 0] == rgb_black[0] &&
			    png_data[index_png + 1] == rgb_black[1] &&
			    png_data[index_png + 2] == rgb_black[2]) {

				printf("x=%d y=%d: black!\n", x, y);

				// TODO: compensate border width
				int index_array = x + (y / 8) * image_width;
				int y_bit = y % 8;

				array_data[index_array] |= (1u << y_bit);
			}
		}
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


