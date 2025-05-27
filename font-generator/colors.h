#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

#define RGB_PIXEL_SIZE   3

#define RGB_BORDER     {0xFF, 0xFF, 0xFF};
#define RGB_BACKGROUND {0xEF, 0xEF, 0xEF};
#define RGB_BLACK      {0x00, 0x00, 0x00};


#define COLOR_EQUALS(lhs, rhs) (\
	(lhs)[0] == (rhs)[0] && \
	(lhs)[1] == (rhs)[1] && \
	(lhs)[2] == (rhs)[2])

#endif // COLORS_H
