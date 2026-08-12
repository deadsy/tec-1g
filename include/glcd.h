//-----------------------------------------------------------------------------
/*

Graphics LCD Driver (st7920)

*/
//-----------------------------------------------------------------------------

#ifndef GLCD_H
#define GLCD_H

//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------

#define GLCD_ROWS 4
#define GLCD_COLS 16

#define GLCD_SPACE 0x20
#define GLCD_LEFT_ARROW 0x1b
#define GLCD_RIGHT_ARROW 0x1a

//-----------------------------------------------------------------------------

void glcd_init(void);

// graphics
void glcd_set_graphics_mode(void);
void glcd_clear_graphics(bool flush);

// text
void glcd_set_text_mode(void);
void glcd_clear_text_row(uint8_t row, bool flush);
void glcd_clear_text(bool flush);
void glcd_flush_text_row(uint8_t row);
void glcd_flush_text(void);
void glcd_puts(uint8_t row, uint8_t col, const char *s, bool flush);
void glcd_putc(uint8_t row, uint8_t col, char c, bool flush);

//-----------------------------------------------------------------------------

#endif				// GLCD_H

//-----------------------------------------------------------------------------
