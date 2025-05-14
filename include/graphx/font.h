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


#define FONT_OFFSET_WIDTH  0
#define FONT_OFFSET_HEIGHT 1
#define FONT_OFFSET_DATA   2

uint8_t font_get_width(const uint8_t *font)
{
#ifdef __AVR__
	return pgm_read_byte(&font[FONT_OFFSET_WIDTH]);
#else
	return font[FONT_OFFSET_WIDTH];
#endif
}

uint8_t font_get_height(const uint8_t *font)
{
#ifdef __AVR__
	return pgm_read_byte(&font[FONT_OFFSET_HEIGHT]);
#else
	return font[FONT_OFFSET_HEIGHT];
#endif
}

uint8_t font_get_byte(const uint8_t *font, uint16_t index)
{
#ifdef __AVR__
	return pgm_read_byte(&font[FONT_OFFSET_DATA + index]);
#else
	return font[FONT_OFFSET_DATA + index];
#endif
}

#endif // FONT_IMPLEMENTATION

#endif // FONT_H
