#ifndef GRAPHX_H
#define GRAPHX_H

/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>

#define GRAPHX_BUFFER_SIZE(width,height)   (width * (height / 8))

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
/*
void graphx_draw_hline(size_t x, size_t y, size_t width, uint8_t color);
void graphx_draw_vline(uint8_t x, uint8_t y, uint8_t height, uint8_t color);
void graphx_draw_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1,
                      uint8_t color);
*/


/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef GRAPHX_IMPLEMENTATION

#include <string.h>

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

//void graphx_draw_char(const uint8_t font[], uint8_t x, uint8_t y, const char c)
//{
//	const uint8_t font_width  = font_get_width(font);
//	const uint8_t font_height = font_get_height(font);
//
//	uint8_t  char_index  = (uint8_t)(c - 0x20);
//	uint16_t start_index = char_index * font_width;
//
//	for (uint8_t row = 0; row < (font_height / 8); row++) {
//
//		uint16_t row_offset = row * font_width;
//
//		for (uint8_t col = 0; col < font_width; col++) {
//
//			uint16_t i_new = start_index + row_offset + col;
//			uint8_t  x_new = x + col;
//			uint8_t  y_new = y + (row * 8);
//
//			graphx_draw_byte(x_new, y_new,
//			                 font_get_byte(font, i_new));
//		}
//	}
//}
//
//void graphx_draw_string(const uint8_t font[], uint8_t x, uint8_t y,
//                        const char *s)
//{
//	const uint8_t font_width = font_get_width(font);
//
//	for (uint8_t i = 0; i < strlen(s); i++) {
//		graphx_draw_char(font, x + (i * (font_width + 1)), y, s[i]);
//	}
//}
//
//void graphx_draw_hline(uint8_t x0, uint8_t x1, uint8_t y, uint8_t color)
//{
//	uint8_t delta = x1 - x0;
//
//	for (uint8_t i = 0; i <= delta; i++) {
//		graphx_draw_pixel(x0 + i, y, color);
//	}
//}
//
//void graphx_draw_vline(uint8_t x, uint8_t y0, uint8_t y1, uint8_t color)
//{
//	uint8_t delta = y1 - y0;
//
//	for (uint8_t i = 0; i <= delta; i++) {
//		graphx_draw_pixel(x, y0 + i, color);
//	}
//}
//
//void graphx_draw_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1,
//                      uint8_t color)
//{
//	graphx_draw_hline(x0, x1, y0, color);
//	graphx_draw_hline(x0, x1, y1, color);
//	graphx_draw_vline(x0, y0, y1, color);
//	graphx_draw_vline(x1, y0, y1, color);
//}
//

#endif // GRAPHX_IMPLEMENTATION

#endif /* GRAPHX_H */
