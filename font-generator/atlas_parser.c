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
#include "util_makros.h"

#if 0
	#define DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
	#define DEBUG_PRINTF(...)
#endif

struct atlas_info {
	int char_width;
	int char_height;
	int chars_per_col;
	int chars_per_row;
	int img_width;
	int img_height;
	uint8_t *img_data;
	int height_bytes_per_char;
	int width_bytes_per_char;
	int index_offset_per_char;
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


static void font_set_pixel_of_char(const struct atlas_info *atlas,
		struct font_data *fdata,
		size_t char_index,
		int char_x,
		int char_y)
{
	const int    y_bit            = char_y % 8;
	const int    offset_y_index   = (char_y / 8) * atlas->width_bytes_per_char;
	const size_t index = char_index + char_x + offset_y_index;

	assert(index < fdata->len);
	fdata->data[index] |= (1 << y_bit);
}

static size_t get_font_data_index_of_char(
		const struct atlas_info *atlas,
		int char_col_index,
		int char_row_index
		)
{
	return (char_row_index * (atlas->chars_per_col* atlas->index_offset_per_char))
		+ (char_col_index * atlas->index_offset_per_char);

}

static void parse_char(
		const struct atlas_info *atlas,
		struct font_data *fdata,
		int char_col_index,
		int char_row_index,
		int img_x_start,
		int img_y_start)
{
	assert(img_x_start > 0);
	assert(img_y_start > 0);
	assert(atlas->char_width  > 0);
	assert(atlas->char_height > 0);
	assert(atlas->img_data != NULL);
	assert(fdata->data     != NULL);


	size_t char_index = get_font_data_index_of_char(atlas, char_col_index, char_row_index);

	DEBUG_PRINTF("\nparse_char: x=%03d y=%03d | col=%03d row=%03d | index=%03zu\n", img_x_start, img_y_start, char_col_index, char_row_index, font_index);

	for (int char_y=0; char_y < atlas->char_height; ++char_y) {
		DEBUG_PRINTF("|");
		for (int char_x=0; char_x < atlas->char_width; ++char_x) {
			if (img_is_pixel_set(atlas, img_x_start+char_x, img_y_start+char_y, atlas->img_data)) {
				DEBUG_PRINTF("#");
				font_set_pixel_of_char(atlas, fdata, char_index, char_x, char_y);
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
		const struct font_data *fdata,
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
	fprintf(f, "\t0x%02X, // data len high byte\n", (uint8_t)(fdata->len >> 8) & 0xFF);
	fprintf(f, "\t0x%02X, // data len low byte\n", (uint8_t)(fdata->len >> 0) & 0xFF);

	for (size_t i=0; i < fdata->len; ++i) {
		if ((i % atlas->char_width) == 0 && (i != 0)) {
			fprintf(f, " // %zu\n\t", i);
		}
		else if (i == 0) {
			fprintf(f, "\t");
		}

		fprintf(f, "0x%02X, ", (int) fdata->data[i]);

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
		.chars_per_col = (img_width  - border_width) / (char_width  + border_width),
		.chars_per_row = (img_height - border_width) / (char_height + border_width),
		.char_width    = char_width,
		.char_height   = char_height,
		.img_width     = img_width,
		.img_height    = img_height,
		.img_data      = img_data,
		.height_bytes_per_char = (char_height/ 8) + 1,
		.width_bytes_per_char  = char_width,
		.index_offset_per_char = atlas.width_bytes_per_char * atlas.height_bytes_per_char
	};

	struct font_data fdata = {
		.data   = NULL,
		.len    = atlas.chars_per_col* atlas.chars_per_row * atlas.height_bytes_per_char * atlas.width_bytes_per_char
	};

	printf("DEBUG: parsing atlas, calculated values:\n"
		"char_columns = %d\n"
		"char_rows    = %d\n"
		"height_bytes_per_char = %d\n"
		"width_bytes_per_char  = %d\n",
		atlas.chars_per_col, atlas.chars_per_row,
		atlas.height_bytes_per_char, atlas.width_bytes_per_char);

	printf("DEBUG: allocated font: size=%zu\n", fdata.len);

	fdata.data = (uint8_t*) malloc(fdata.len);

	if (fdata.data == NULL) {
		return result_make_error(errno);
	}

	memset(fdata.data, 0, fdata.len);

	for (int row=0; row < atlas.chars_per_row; ++row) {
		const int y_start = border_width + (row * (char_height + border_width));

		for (int col=0; col <  atlas.chars_per_col; ++col) {
			const int x_start = border_width + (col * (char_width + border_width));

			parse_char(&atlas, &fdata, col, row, x_start, y_start);
		}
	}
	stbi_image_free(img_data);

	return write_font_file(&atlas, &fdata, output_file);
}



