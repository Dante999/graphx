#ifndef GRAPHX_H
#define GRAPHX_H

/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define GRAPHX_BUFFER_SIZE(width,height)   (width * height / 8)

#ifdef GRAPHX_ENABLE_DEBUG
	#include <stdio.h>
	#define GRAPHX_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
	#define GRAPHX_PRINTF(...)
#endif


enum graphx_orientation {
	GRAPHX_ORIENTATION_VERTICAL,
	GRAPHX_ORIENTATION_HORIZONTAL
};

struct graphx_data {
	uint8_t *buffer;
	uint16_t buffer_size;
	uint16_t width;
	uint16_t height;
	enum graphx_orientation orientation;
};

enum graphx_color {
	GRAPHX_COLOR_WHITE,
	GRAPHX_COLOR_BLACK,
	GRAPHX_COLOR_TOGGLE
};

enum graphx_error {
	GRAPHX_ERROR_OK,
	GRAPHX_ERROR_NULLPTR,
	GRAPHX_ERROR_BUFFER_NULL,
	GRAPHX_ERROR_INVALID_BUFFER_SIZE,
	GRAPHX_ERROR_WIDTH_NO_MULTIPLE_OF_8,
	GRAPHX_ERROR_HEIGHT_NO_MULTIPLE_OF_8,
};


enum graphx_error graphx_verify(const struct graphx_data *data);

enum graphx_color graphx_get_pixel(const struct graphx_data *data, uint16_t x, uint16_t y);

void graphx_fill(struct graphx_data *data, enum graphx_color color);
void graphx_draw_pixel(struct graphx_data *data, uint16_t x, uint16_t y, enum graphx_color color);
void graphx_draw_hline(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t width, enum graphx_color color);
void graphx_draw_vline(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t height, enum graphx_color color);
void graphx_draw_rect(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height, enum graphx_color color);
void graphx_draw_char(struct graphx_data *data,const uint8_t *font, uint16_t x, uint16_t y, char c, enum graphx_color color);
void graphx_draw_symbol(struct graphx_data *data,const uint8_t *font, uint16_t x, uint16_t y, char index, enum graphx_color color);
void graphx_draw_string(struct graphx_data *data,const uint8_t *font, uint16_t x, uint16_t y, const char *s, enum graphx_color color);


/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef GRAPHX_IMPLEMENTATION

#include <string.h>

#define FONT_IMPLEMENTATION
#include "font.h"

static void graphx_draw_byte(struct graphx_data *data, uint16_t x, uint16_t y,
	uint8_t byte, uint8_t bit_count, enum graphx_color fg_color, enum graphx_color bg_color)
{
	for (uint8_t j = 0; j < bit_count; j++) {
		enum graphx_color color = (byte & (1 << j)) ? fg_color : bg_color;

		if (color == fg_color) {
			GRAPHX_PRINTF("draw_pixel: x=%03d y=%03d\n", x, y+j);
		}
		graphx_draw_pixel(data, x, y + j, color);
	}
}


static uint8_t graphx_get_bitmask(const struct graphx_data *data, uint16_t x, uint16_t y)
{
	assert(x < data->width);
	assert(y < data->height);

	switch(data->orientation) {
		case GRAPHX_ORIENTATION_VERTICAL  : (void)x; return (y%8);
		case GRAPHX_ORIENTATION_HORIZONTAL: (void)y; return (x%8);
	};
}

static uint16_t graphx_get_buffer_offset(const struct graphx_data *data, uint16_t x, uint16_t y)
{
	assert(x < data->width);
	assert(y < data->height);

	uint16_t offset = UINT16_MAX;

	switch(data->orientation) {
		case GRAPHX_ORIENTATION_VERTICAL:
			offset = x + (y / 8u) * data->width;
			break;
		case GRAPHX_ORIENTATION_HORIZONTAL:
			offset = (x/8u) + y * (data->width/8);
			break;
		default:
			assert(false);
			break;
	}

	assert(offset < data->buffer_size);
	return offset;
}


enum graphx_error graphx_verify(const struct graphx_data *data)
{
	if (data == NULL) {
		return GRAPHX_ERROR_NULLPTR;
	}
	if (data->width % 8 != 0) {
		return GRAPHX_ERROR_WIDTH_NO_MULTIPLE_OF_8;
	}
	if (data->height% 8 != 0) {
		return GRAPHX_ERROR_HEIGHT_NO_MULTIPLE_OF_8;
	}
	if (data->buffer == NULL) {
		return GRAPHX_ERROR_BUFFER_NULL;
	}
	if (data->buffer_size != GRAPHX_BUFFER_SIZE(data->width, data->height)) {
		return GRAPHX_ERROR_INVALID_BUFFER_SIZE;
	}

	return GRAPHX_ERROR_OK;
}


void graphx_fill(struct graphx_data *data, enum graphx_color color)
{
	const size_t buffer_size = GRAPHX_BUFFER_SIZE(data->width,data->height);

	switch(color) {

	case GRAPHX_COLOR_BLACK:
		memset(data->buffer, 0xFF, buffer_size);
		break;

	case GRAPHX_COLOR_WHITE:
		memset(data->buffer, 0x00, buffer_size);
		break;

	case GRAPHX_COLOR_TOGGLE:
		for( size_t i=0; i < buffer_size; ++i) {
			data->buffer[i] ^= 0xFF;
		}
		break;
	}
}



enum graphx_color graphx_get_pixel(const struct graphx_data *data, uint16_t x, uint16_t y)
{
	const uint8_t  bmask  = graphx_get_bitmask(data, x, y);
	const uint16_t offset = graphx_get_buffer_offset(data, x, y);

	return (data->buffer[offset] & (1 << bmask)) ? GRAPHX_COLOR_BLACK : GRAPHX_COLOR_WHITE;
}

void graphx_draw_pixel(struct graphx_data *data, uint16_t x, uint16_t y, enum graphx_color color)
{
	assert(x < data->width);
	assert(y < data->height);

	const uint8_t  bmask  = graphx_get_bitmask(data, x, y);
	const uint16_t offset = graphx_get_buffer_offset(data, x, y);

	switch(color) {

	case GRAPHX_COLOR_BLACK:
		data->buffer[offset] |= (1u << bmask);
		break;

	case GRAPHX_COLOR_WHITE:
		data->buffer[offset] &= ~(1u << bmask);
		break;

	case GRAPHX_COLOR_TOGGLE:
		data->buffer[offset] ^= (1u << bmask);
		break;
	}
}

void graphx_draw_hline(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t width, enum graphx_color color)
{
	for (uint16_t i = 0; i <= width; ++i) {
		graphx_draw_pixel(data, x + i, y, color);
	}
}

void graphx_draw_vline(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t height, enum graphx_color color)
{
	for (uint16_t i = 0; i <= height; i++) {
		graphx_draw_pixel(data, x, y + i, color);
	}
}

void graphx_draw_rect(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height, enum graphx_color color)
{
	graphx_draw_hline(data, x         , y         , width , color);
	graphx_draw_hline(data, x         , y + height, width , color);
	graphx_draw_vline(data, x         , y         , height, color);
	graphx_draw_vline(data, x + width , y         , height, color);
}


void graphx_draw_symbol(struct graphx_data *data,const uint8_t *font, uint16_t x, uint16_t y, char index, enum graphx_color color)
{
	enum graphx_color fg_color = GRAPHX_COLOR_BLACK;
	enum graphx_color bg_color = GRAPHX_COLOR_WHITE;

	switch(color) {
	case GRAPHX_COLOR_BLACK:
		// already set
		 break;
	case GRAPHX_COLOR_WHITE:
		 fg_color = GRAPHX_COLOR_WHITE;
		 bg_color = GRAPHX_COLOR_BLACK;
		 break;
	case GRAPHX_COLOR_TOGGLE:
		fg_color = GRAPHX_COLOR_TOGGLE;
		fg_color = GRAPHX_COLOR_TOGGLE;
		break;
	}

	const uint8_t font_width  = font_get_width(font);
	const uint8_t font_height = font_get_height(font);

	const uint16_t height_bytes_per_char = (font_height / 8) + 1;
	const uint16_t width_bytes_per_char  = font_width;

	const uint16_t start_index = index * width_bytes_per_char * height_bytes_per_char;

	for (uint16_t row = 0; row < height_bytes_per_char; row++) {

		uint16_t row_offset          = row * width_bytes_per_char;
		uint16_t y_offset            = row * 8;
		uint16_t height_bits_to_draw = 8;

		if (y_offset+8 > font_height) {
			height_bits_to_draw = font_height % 8;
		}

		for (uint16_t col= 0; col < width_bytes_per_char; col++) {

			size_t font_byte_index = start_index + row_offset + col;
#if 0
			GRAPHX_PRINTF("char=%c width=%02d height=%02d row_offset=%03d y_offset=%03d height_bits_to_draw=%03d "
					"index=%zu x=%03d y=%03d\n",
				c, font_width, font_height, (int)row_offset, (int)y_offset, (int)height_bits_to_draw,
				font_byte_index, x, y);
#endif
			graphx_draw_byte(
				data,
				x + col,
				y + y_offset,
				font_get_data_byte(font, font_byte_index),
				height_bits_to_draw,
				fg_color,
				bg_color);
		}
	}
}

void graphx_draw_char(
	struct graphx_data *data,
	const uint8_t *font,
	uint16_t x,
	uint16_t y,
	char c,
	enum graphx_color color)
{
	graphx_draw_symbol(data, font, x, y, (c - 0x20), color);
}

void graphx_draw_string(
	struct graphx_data *data,
	const uint8_t *font,
	uint16_t x,
	uint16_t y,
	const char *s,
	enum graphx_color color)
{
	const uint8_t font_width = font_get_width(font);

	for (uint8_t i = 0; i < strlen(s); i++) {
		graphx_draw_char(data, font, x + (i * (font_width + 1)), y, s[i], color);
	}
}

#endif // GRAPHX_IMPLEMENTATION

#endif /* GRAPHX_H */
