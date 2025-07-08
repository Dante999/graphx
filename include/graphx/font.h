#ifndef FONT_H
#define FONT_H

/* *****************************************************************************
 * API
 ******************************************************************************/
#include <stdint.h>

uint8_t font_get_width(const uint8_t *font);
uint8_t font_get_height(const uint8_t *font);
uint8_t font_get_byte(const uint8_t *font, uint16_t index);


/* *****************************************************************************
 * Implementation
 ******************************************************************************/
#ifdef FONT_IMPLEMENTATION

#ifdef __AVR__
	#include <avr/pgmspace.h>
#endif


#define FONT_INDEX_WIDTH_BYTE       0
#define FONT_INDEX_HEIGHT_BYTE      1
#define FONT_INDEX_LENGTH_BYTE_HIGH 2
#define FONT_INDEX_LENGTH_BYTE_LOW  3
#define FONT_INDEX_DATA_START       4

uint8_t font_get_width(const uint8_t *font)
{
#ifdef __AVR__
	return pgm_read_byte(&font[FONT_INDEX_WIDTH_BYTE]);
#else
	return font[FONT_INDEX_WIDTH_BYTE];
#endif
}

uint8_t font_get_height(const uint8_t *font)
{
#ifdef __AVR__
	return pgm_read_byte(&font[FONT_INDEX_HEIGHT_BYTE]);
#else
	return font[FONT_INDEX_HEIGHT_BYTE];
#endif
}

uint16_t font_get_data_len(const uint8_t *font)
{
	uint16_t len = 0;
#ifdef __AVR__
	len |= (pgm_read_byte(&font[FONT_INDEX_LENGTH_BYTE_LOW])  << 0);
	len |= (pgm_read_byte(&font[FONT_INDEX_LENGTH_BYTE_HIGH]) << 8);
#else
	len |= (font[FONT_INDEX_LENGTH_BYTE_LOW]  << 0);
	len |= (font[FONT_INDEX_LENGTH_BYTE_HIGH] << 8);
#endif

	return len;
}

uint8_t font_get_data_byte(const uint8_t *font, uint16_t index)
{
#ifdef __AVR__
	return pgm_read_byte(&font[FONT_INDEX_DATA_START + index]);
#else
	return font[FONT_INDEX_DATA_START + index];
#endif
}

#endif // FONT_IMPLEMENTATION

#endif // FONT_H
