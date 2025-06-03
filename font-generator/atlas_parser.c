#include "atlas_parser.h"

#include "colors.h"

#include <assert.h>
#include <stdint.h>
#include <strings.h>
#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image.h"

#define UTIL_PATHS_IMPLEMENTATION
#include "util_paths.h"

#include "util_strings.h"


#if 0
	#define DEBUG_PRINTF(c) printf(c)
#else
	#define DEBUG_PRINTF(...)
#endif

struct atlas_info {
	int char_width;
	int char_height;
	int char_columns;
	int char_rows;
	int img_width;
	int img_height;
};

static const uint8_t rgb_black[RGB_PIXEL_SIZE] = RGB_BLACK;


static bool img_is_pixel_set(const struct atlas_info *info, int x, int y, const uint8_t *img_data)
{
	assert(info != NULL);

	int png_index = (y * info->img_width + x) * RGB_PIXEL_SIZE;

	if (COLOR_EQUALS(&img_data[png_index], &rgb_black[0])) {
		return true;
	}
	else {
		return false;
	}
}

static void array_set_pixel(const struct atlas_info *info, int col, int row,
		int x, int y, uint8_t *array_data)
{
	const int start_index = (info->char_width * col) + (info->char_columns * info->char_width * row);

	array_data[start_index+x] |= (1 << y);
}


static void parse_char(
		const struct atlas_info *info,
		int col,
		int row,
		int x_start,
		int y_start,
		const uint8_t *img_data,
		uint8_t *array_data)
{
	assert(x_start > 0);
	assert(y_start > 0);
	assert(info->char_width  > 0);
	assert(info->char_height > 0);
	assert(img_data   != NULL);
	assert(array_data != NULL);

	DEBUG_PRINTF("parse_char: x=%03d y=%03d\n", x_start, y_start);

	for (int y=0; y < info->char_height; ++y) {
		DEBUG_PRINTF("|");
		for (int x=0; x < info->char_width; ++x) {
			if (img_is_pixel_set(info, x_start+x, y_start+y, img_data)) {
				DEBUG_PRINTF("#");
				array_set_pixel(info, col, row, x, y, array_data);
			}
			else {
				DEBUG_PRINTF(" ");
			}
		}
		DEBUG_PRINTF("|\n");
	}
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

	char header_guard[255];
	util_filename_from_path(output_file, header_guard);
	util_strtoupper(header_guard);
	util_strreplace(header_guard, '.', '_');

	char variable_name[255];
	util_filename_from_path(output_file, variable_name);
	util_remove_file_extension(variable_name);


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

	int img_width  = 0;
	int img_height = 0;
	int channels = 0;

	uint8_t *img_data = stbi_load(input_file, &img_width, &img_height, &channels, 0);

	if (channels != RGB_PIXEL_SIZE) {
		return result_make(
				false,
				"expected png color channels to be %d but is %d",
				RGB_PIXEL_SIZE, channels);
	}

	const struct atlas_info info = {
		.char_columns = (img_width  - border_width) / (char_width  + border_width),
		.char_rows    = (img_height - border_width) / (char_height + border_width),
		.char_width   = char_width,
		.char_height  = char_height,
		.img_width    = img_width,
		.img_height   = img_height
	};

	printf("DEBUG: parsing atlas, calculated char_columns=%d char_rows=%d\n", info.char_columns, info.char_rows);
	const size_t array_size = info.char_columns * info.char_rows * char_height* char_width;
	assert(array_size > 0);
	printf("DEBUG: array_size=%zu\n", array_size);

	uint8_t *array_data = (uint8_t*) malloc(array_size);

	if (array_data == NULL) {
		return result_make_error(errno);
	}
	memset(array_data, 0, array_size);

	for (int row=0; row < info.char_rows; ++row) {
		const int y_start = border_width + (row * (char_height + border_width));

		for (int col=0; col <  info.char_columns; ++col) {
			const int x_start = border_width + (col * (char_width + border_width));

			parse_char(&info, col, row, x_start, y_start, img_data, array_data);
		}
	}
	stbi_image_free(img_data);

	return write_font_file(output_file, char_width, char_height, array_data, array_size);
}



