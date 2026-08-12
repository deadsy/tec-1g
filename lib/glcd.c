//-----------------------------------------------------------------------------
/*

Graphics LCD Driver (st7920)

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "glcd.h"
#include "hw.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define GLCD_PIXELS_H 128
#define GLCD_PIXELS_V 64

//-----------------------------------------------------------------------------

// RE = 0 instruction set
#define cmdClear					(0x01)
#define cmdHome						(0x02)
#define cmdEntryMode(id, s)			(0x04 | ((id) << 1) | ((s) << 0))
#define cmdDisplayCtrl(d,c,b)		(0x08 | ((d) << 2) | ((c) << 1) | ((b) << 0) )
#define cmdCursorControl(sc, rl)	(0x10 | ((sc) << 3) | ((rl) << 2))
#define cmdFunctionSet(dl, re) 		(0x20 | ((dl) << 4) | ((re) << 2))
#define cmdCgramAddress(ac) 		(0x40 | ((ac) & 0x3f))
#define cmdDdramAddress(ac)			(0x80 | ((ac) & 0x3f))

// RE = 1 instruction set
#define cmdStandBy						(0x01)
#define cmdScrollSelect(sr)				(0x02 | ((sr) << 0))
#define cmdReverse(r)					(0x04 | ((r) & 3))
#define cmdExtFunctionSet(dl, re, g) 	(0x20 | ((dl) << 4) | ((re) << 2) | ((g) << 1))
#define cmdIramAddress(ac)				(0x40 | ((ac) & 0x3f))
#define cmdGdramAddress(ac)				(0x80 | ((ac) & 0x7f))

#define BUSYFLAG 0x80

//-----------------------------------------------------------------------------

static void wr_command(uint8_t cmd) {
	glcdCommandPort = cmd;
	while ((glcdCommandPort & BUSYFLAG) != 0) ;
}

static void wr_data(uint8_t val) {
	glcdDataPort = val;
	while ((glcdCommandPort & BUSYFLAG) != 0) ;
}

//-----------------------------------------------------------------------------

void glcd_set_text_mode(void) {
	wr_command(cmdFunctionSet(1, 0));
}

void glcd_set_graphics_mode(void) {
	wr_command(cmdFunctionSet(1, 1));
	wr_command(cmdExtFunctionSet(1, 1, 1));
}

//-----------------------------------------------------------------------------
// graphics (gdram) operations

// graphics buffer
static uint8_t glcd_gbuf[(GLCD_PIXELS_H * GLCD_PIXELS_V) >> 3];

void glcd_clear_graphics(bool flush) {
	memset(glcd_gbuf, 0, sizeof(glcd_gbuf));
	if (flush) {
		glcd_set_graphics_mode();
		// TODO
	}
}

//-----------------------------------------------------------------------------
// Text (ddram) operations.
// Text operations write to a RAM buffer (glcd_tbuf).
// The buffer is then flushed to the st7920 upon command.

// text buffer
static uint8_t glcd_tbuf[GLCD_ROWS][GLCD_COLS];

// return true for a valid position in the text buffer
static inline bool valid_text_posn(uint8_t row, uint8_t col) {
	return (row < GLCD_ROWS) && (col < GLCD_COLS);
}

// set the ddram address to the start of a row
static const uint8_t row_map[4] = { 0, 2, 1, 3 };
static inline void set_ddram_row(uint8_t row) {
	wr_command(cmdDdramAddress(row_map[row] << 3));
}

// flush a single row to the st7920 (text mode is set)
static void flush_text_row(uint8_t row) {
	set_ddram_row(row);
	uint8_t *ptr = &glcd_tbuf[row][0];
	for (uint8_t i = 0; i < GLCD_COLS; i++) {
		wr_data(*ptr++);
	}
}

// flush a single row to the st7920
void glcd_flush_text_row(uint8_t row) {
	glcd_set_text_mode();
	flush_text_row(row);
}

// flush all text rows to the st7920
void glcd_flush_text(void) {
	glcd_set_text_mode();
	flush_text_row(0);
	flush_text_row(1);
	flush_text_row(2);
	flush_text_row(3);
}

// clear a text row
void glcd_clear_text_row(uint8_t row, bool flush) {
	uint8_t *ptr = &glcd_tbuf[row][0];
	memset(ptr, GLCD_SPACE, GLCD_COLS);
	if (flush) {
		flush_text_row(row);
	}
}

// clear the text screen
void glcd_clear_text(bool flush) {
	memset(glcd_tbuf, GLCD_SPACE, sizeof(glcd_tbuf));
	if (flush) {
		glcd_set_text_mode();
		wr_command(cmdClear);
	}
}

// write a string to the text buffer (no line wrapping)
void glcd_puts(uint8_t row, uint8_t col, const char *s, bool flush) {
	if (!valid_text_posn(row, col)) {
		return;
	}
	uint8_t *ptr = &glcd_tbuf[row][col];
	while ((*s != 0) && (col < GLCD_COLS)) {
		*ptr++ = *s++;
		col++;
	}
	if (flush) {
		glcd_flush_text_row(row);
	}
}

// write a character to the text buffer
void glcd_putc(uint8_t row, uint8_t col, char c, bool flush) {
	if (!valid_text_posn(row, col)) {
		return;
	}
	glcd_tbuf[row][col] = c;
	if (flush) {
		glcd_flush_text_row(row);
	}
}

//-----------------------------------------------------------------------------

void glcd_init(void) {
	delay_ms(40);
	glcdCommandPort = cmdFunctionSet(1, 0);
	delay_125us();
	glcdCommandPort = cmdFunctionSet(1, 0);
	delay_125us();
	glcdCommandPort = cmdDisplayCtrl(1, 0, 0);
	delay_125us();
	glcdCommandPort = cmdClear;
	delay_ms(11);
	glcdCommandPort = cmdEntryMode(1, 0);

	glcd_clear_text(false);
	glcd_clear_graphics(false);
}

//-----------------------------------------------------------------------------
