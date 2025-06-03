#ifndef GRAPHX_H
#define GRAPHX_H

/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define GRAPHX_BUFFER_SIZE(width,height)   (width * height / 8)

struct graphx_data {
	uint8_t *buffer;
	uint16_t buffer_size;
	uint16_t width;
	uint16_t height;
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
	GRAPHX_ERROR_INVALID_BUFFER_SIZE
};


enum graphx_error graphx_verify(const struct graphx_data *data);

enum graphx_color graphx_get_pixel(const struct graphx_data *data, uint16_t x, uint16_t y);

void graphx_fill(struct graphx_data *data, enum graphx_color color);
void graphx_draw_pixel(struct graphx_data *data, uint16_t x, uint16_t y, enum graphx_color color);
void graphx_draw_hline(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t width, enum graphx_color color);
void graphx_draw_vline(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t height, enum graphx_color color);
void graphx_draw_rect(struct graphx_data *data, uint16_t x, uint16_t y, uint16_t width, uint16_t height, enum graphx_color color);
void graphx_draw_char(struct graphx_data *data,const uint8_t *font, uint16_t x, uint16_t y, char c, enum graphx_color color);
void graphx_draw_string(struct graphx_data *data,const uint8_t *font, uint16_t x, uint16_t y, const char *s, enum graphx_color color);


/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef GRAPHX_IMPLEMENTATION

#include <string.h>

#define FONT_IMPLEMENTATION
#include "font.h"

static void graphx_draw_byte(struct graphx_data *data, uint16_t x, uint16_t y,
	uint8_t byte, enum graphx_color fg_color, enum graphx_color bg_color)
{
	for (uint8_t j = 0; j < 8; j++) {
		enum graphx_color color = (byte & (1 << j)) ? fg_color : bg_color;
		graphx_draw_pixel(data, x, y + j, color);
	}
}


static uint16_t graphx_get_buffer_offset(const struct graphx_data *data, uint16_t x, uint16_t y)
{
	const uint16_t offset = x + (y / 8u) * data->width;
	return (offset > data->buffer_size) ? data->buffer_size: offset;
}


enum graphx_error graphx_verify(const struct graphx_data *data)
{
	if (data == NULL) {
		return GRAPHX_ERROR_NULLPTR;
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
	const uint8_t  y_bit  = y % 8;
	const uint16_t offset = graphx_get_buffer_offset(data, x, y);

	return (data->buffer[offset] & (1 << y_bit)) ? GRAPHX_COLOR_BLACK : GRAPHX_COLOR_WHITE;
}

void graphx_draw_pixel(struct graphx_data *data, uint16_t x, uint16_t y, enum graphx_color color)
{
	assert(x < data->width);
	assert(y < data->height);

	const uint8_t  y_bit  = y % 8;
	const uint16_t offset = graphx_get_buffer_offset(data, x, y);

	switch(color) {

	case GRAPHX_COLOR_BLACK:
		data->buffer[offset] |= (1u << y_bit);
		break;

	case GRAPHX_COLOR_WHITE:
		data->buffer[offset] &= ~(1u << y_bit);
		break;

	case GRAPHX_COLOR_TOGGLE:
		data->buffer[offset] ^= (1u << y_bit);
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


void graphx_draw_char(
	struct graphx_data *data,
	const uint8_t *font,
	uint16_t x,
	uint16_t y,
	char c,
	enum graphx_color color)
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

	uint8_t  char_index  = (uint8_t)(c - 0x20);
	uint16_t start_index = char_index * width_bytes_per_char * height_bytes_per_char;

	for (uint16_t row = 0; row < height_bytes_per_char; row++) {

		uint16_t row_offset = row * width_bytes_per_char;

		for (uint16_t col= 0; col < width_bytes_per_char; col++) {

			uint16_t i_new = start_index + row_offset + col;

			uint16_t  x_new = x + col;
			uint16_t  y_new = y + (row * 8);

			graphx_draw_byte(data, x_new, y_new,
				font_get_byte(font, i_new), fg_color, bg_color);
		}
	}
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
