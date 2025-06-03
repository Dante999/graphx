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


#if 1
	#define DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__) 
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
	uint8_t *img_data;
};

struct font_data {
	uint8_t *data;
	size_t   len;
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

static void font_set_pixel_of_char(
		const struct atlas_info *atlas,
		struct font_data *fdata,
		int col,
		int row,
		int x,
		int y)
{
	//FIXME: y is not correctly handled for char width a height more than
	// one byte
#if 1
	const int start_index = (atlas->char_width * col) + (atlas->char_columns * atlas->char_width * row);
	const int y_bit       = y % 8;

	const size_t index = start_index+x;
	assert(index < fdata->len);
	fdata->data[index] |= (1 << y_bit);
#else
	////const int height_bytes_per_char = (y / 8) + 1;
	//const int width_bytes_per_char  = atlas->char_width;
	//const int y_bit       = y % 8;

	//const size_t start_index = row * col * atlas->char_columns;


	//const size_t row_offset = row * width_bytes_per_char;
/*	//const size_t start_index = row_offset + 
	//	(atlas->char_width * col) + 
	//	(info->char_columns * info->char_width * row * height_byte) +
	//	x;*/

	//const size_t index = start_index+row_offset+x;
	//assert(index < fdata->len);
	//fdata->data[index] |= (1 << y_bit);
#endif
}


static void parse_char(
		const struct atlas_info *atlas,
		struct font_data *fdata,
		int col,
		int row,
		int x_start,
		int y_start)
{
	assert(x_start > 0);
	assert(y_start > 0);
	assert(atlas->char_width  > 0);
	assert(atlas->char_height > 0);
	assert(atlas->img_data != NULL);
	assert(fdata->data     != NULL);

	DEBUG_PRINTF("parse_char: x=%03d y=%03d\n", x_start, y_start);

	for (int y=0; y < atlas->char_height; ++y) {
		DEBUG_PRINTF("|");
		for (int x=0; x < atlas->char_width; ++x) {
			if (img_is_pixel_set(atlas, x_start+x, y_start+y, atlas->img_data)) {
				DEBUG_PRINTF("#");
				font_set_pixel_of_char(atlas, fdata, col, row, x, y);
			}
			else {
				DEBUG_PRINTF(" ");
			}
		}
		DEBUG_PRINTF("|\n");
	}
}

static Result write_font_file(
		const struct atlas_info *atlas,
		const struct font_data  *fdata,
		const char *output_file
		)
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

	fprintf(f, "\t%d, // font width\n", atlas->char_width);
	fprintf(f, "\t%d, // font height\n", atlas->char_height);

	for (size_t i=0; i < fdata->len; ++i) {
		if ((i % atlas->char_width) == 0 && (i != 0)) {
			fprintf(f, "\n\t");
		}
		else if (i == 0) {
			fprintf(f, "\t");
		}

		fprintf(f, "0x%02x, ", (int) fdata->data[i]);

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

	const struct atlas_info atlas = {
		.char_columns = (img_width  - border_width) / (char_width  + border_width),
		.char_rows    = (img_height - border_width) / (char_height + border_width),
		.char_width   = char_width,
		.char_height  = char_height,
		.img_width    = img_width,
		.img_height   = img_height,
		.img_data     = img_data
	};

	struct font_data fdata = {
		.data = NULL,
		.len  = atlas.char_columns * atlas.char_rows * atlas.char_height * atlas.char_width
	};

	printf("DEBUG: parsing atlas, calculated char_columns=%d char_rows=%d\n", 
		atlas .char_columns, atlas.char_rows);
	
	printf("DEBUG: allocated font: size=%zu\n", fdata.len);

	fdata.data = (uint8_t*) malloc(fdata.len);

	if (fdata.data == NULL) {
		return result_make_error(errno);
	}

	memset(fdata.data, 0, fdata.len);

	for (int row=0; row < atlas.char_rows; ++row) {
		const int y_start = border_width + (row * (char_height + border_width));

		for (int col=0; col <  atlas.char_columns; ++col) {
			const int x_start = border_width + (col * (char_width + border_width));

			parse_char(&atlas, &fdata, col, row, x_start, y_start);
		}
	}
	stbi_image_free(img_data);

	return write_font_file(&atlas, &fdata, output_file);
}



