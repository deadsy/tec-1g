//-----------------------------------------------------------------------------
/*

Graphics LCD Driver (st7920)

Note: The st7920 gdram is 256 x 64 pixels. The display size is 128 x 64 pixels.

This is the gdram arrangement:
A B
C D
where each A,B,C,D are each 128 x 32 pixels.

This is the gdram to display mapping:
A
B

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "glcd.h"
#include "hw.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define BYTES_PER_ROW (GLCD_PIXELS_H >> 3)
#define HALF_Y (GLCD_PIXELS_V >> 1)

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

// return true for a valid position in the graphics buffer
static inline bool valid_graphics_posn(uint8_t x, uint8_t y) {
	return (x < GLCD_PIXELS_H) && (y < GLCD_PIXELS_V);
}

// set the gdram address to the start of a y-row.
static inline void set_gdram_adr(uint8_t y) {
	wr_command(cmdGdramAddress(y));
	wr_command(cmdGdramAddress(0));
}

// flush the graphics buffer to the device
void glcd_flush_graphics(void) {
	glcd_set_graphics_mode();
	for (uint8_t y = 0; y < HALF_Y; y++) {
		set_gdram_adr(y);
		// top half of display
		uint8_t *ptr = &glcd_gbuf[y * BYTES_PER_ROW];
		for (uint8_t i = 0; i < (BYTES_PER_ROW >> 1); i++) {
			wr_data(*ptr++);
			wr_data(*ptr++);
		}
		// bottom half of display
		ptr = &glcd_gbuf[(y + HALF_Y) * BYTES_PER_ROW];
		for (uint8_t i = 0; i < (BYTES_PER_ROW >> 1); i++) {
			wr_data(*ptr++);
			wr_data(*ptr++);
		}
	}
}

// clear the graphics buffer (and optionally the device)
void glcd_clear_graphics(bool flush) {
	memset(glcd_gbuf, 0, sizeof(glcd_gbuf));
	if (flush) {
		glcd_set_graphics_mode();
		for (uint8_t y = 0; y < HALF_Y; y++) {
			set_gdram_adr(y);
			for (uint8_t i = 0; i < (BYTES_PER_ROW << 1); i++) {
				wr_data(0);
				wr_data(0);
			}
		}
	}
}

// plot a pixel in the graphics buffer
void glcd_plot(uint8_t x, uint8_t y) {
	if (!valid_graphics_posn(x, y)) {
		return;
	}
	glcd_gbuf[(y * BYTES_PER_ROW) + (x >> 3)] |= 1 << (7 - (x & 7));
}

// plot a vertical line in the graphics buffer
void glcd_vline(uint8_t y0, uint8_t y1, uint8_t x) {
	// swap y0/y1 so y0 <= y1
	if (y0 > y1) {
		uint8_t tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	// single pixel?
	if (y0 == y1) {
		glcd_plot(x, y0);
		return;
	}
	if (!valid_graphics_posn(x, y0)) {
		// both points not on display
		return;
	}
	if (!valid_graphics_posn(x, y1)) {
		// clamp y1 to the edge
		y1 = GLCD_PIXELS_V - 1;
	}
	uint8_t bit = 1 << (7 - (x & 7));
	uint16_t ofs = (y0 * BYTES_PER_ROW) + (x >> 3);
	for (uint8_t i = y0; i <= y1; i++) {
		glcd_gbuf[ofs] |= bit;
		ofs += BYTES_PER_ROW;
	}
}

// plot a horizontal line in the graphics buffer
void glcd_hline(uint8_t x0, uint8_t x1, uint8_t y) {
	// swap x0/x1 so x0 <= x1
	if (x0 > x1) {
		uint8_t tmp = x1;
		x1 = x0;
		x0 = tmp;
	}
	// single pixel?
	if (x0 == x1) {
		glcd_plot(x0, y);
		return;
	}
	if (!valid_graphics_posn(x0, y)) {
		// both points not on display
		return;
	}
	if (!valid_graphics_posn(x1, y)) {
		// clamp x1 to the edge
		x1 = GLCD_PIXELS_H - 1;
		return;
	}

	uint8_t *ptr = &glcd_gbuf[y * BYTES_PER_ROW];
	uint8_t b0 = x0 >> 3;
	uint8_t b1 = x1 >> 3;

	if (b0 == b1) {
		// head and tail in same byte
		uint8_t n = x1 - x0 + 1;
		ptr[b0] |= ((1 << n) - 1) << (7 - (x1 & 7));
	} else {
		// head and tail in different bytes
		// head
		uint8_t n = 8 - (x0 & 7);
		ptr[b0] |= ((1 << n) - 1);
		// tail
		n = 1 + (x1 & 7);
		ptr[b1] |= ((1 << n) - 1) << (8 - n);
		// body (could be empty)
		memset(&ptr[b0 + 1], 0xff, b1 - b0 - 1);
	}
}

// plot a box in the graphics buffer
// x0/y0 = top/left
// x1/y1 = bottom/right
void glcd_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	glcd_hline(x0, x1, y0);
	glcd_hline(x0, x1, y1);
	glcd_vline(y0, y1, x0);
	glcd_vline(y0, y1, x1);
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
	for (uint8_t i = 0; i < GLCD_ROWS; i++) {
		flush_text_row(i);
	}
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
void glcd_puts(uint8_t row, uint8_t col, const char *s) {
	if (!valid_text_posn(row, col)) {
		return;
	}
	uint8_t *ptr = &glcd_tbuf[row][col];
	while ((*s != 0) && (col < GLCD_COLS)) {
		*ptr++ = *s++;
		col++;
	}
}

// write a character to the text buffer
void glcd_putc(uint8_t row, uint8_t col, char c) {
	if (!valid_text_posn(row, col)) {
		return;
	}
	glcd_tbuf[row][col] = c;
}

//-----------------------------------------------------------------------------

// clear the text and graphics buffers (optionally flush)
void glcd_clear(bool flush) {
	glcd_clear_text(flush);
	glcd_clear_graphics(flush);
}

// flush the text and graphics buffers.
void glcd_flush(void) {
	glcd_flush_text();
	glcd_flush_graphics();
}

// initialise the st7920
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

	glcd_clear(false);
}

//-----------------------------------------------------------------------------
