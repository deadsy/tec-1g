//-----------------------------------------------------------------------------
/*

Graphics LCD Driver (st7920)

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdbool.h>

#include "glcd.h"
#include "hw.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define GLCD_ROWS 4
#define GLCD_COLS 16

#define GLCD_PIXELS_H 128
#define GLCD_PIXELS_V 64

#define GLCD_SPACE 0x20

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

// graphics buffer
static uint8_t gbuf[(GLCD_PIXELS_H * GLCD_PIXELS_V) >> 3];

static void gbuf_clear() {
	memset(gbuf, 0, sizeof(gbuf));
}

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

// text buffer
static uint8_t tbuf[GLCD_ROWS * GLCD_COLS];

static void tbuf_clear() {
	memset(tbuf, GLCD_SPACE, sizeof(tbuf));
}

static inline bool valid_ddram_posn(uint8_t row, uint8_t col) {
	return (row < GLCD_ROWS) && (col < GLCD_COLS);
}

static const uint8_t row_map[4] = { 0, 2, 1, 3 };

static inline void set_ddram_posn(uint8_t row, uint8_t col) {
	// note: col is /2 implying the col is 0,2,4...
	wr_command(cmdDdramAddress((row_map[row] << 3) | (col >> 1)));
}

void glcd_puts(uint8_t row, uint8_t col, const char *s) {
	// we always start at an even column
	col &= ~1;
	if (!valid_ddram_posn(row, col)) {
		return;
	}
	glcd_set_text_mode();
	set_ddram_posn(row, col);
	uint8_t n = 0;
	while ((*s != 0) && (col < GLCD_COLS)) {
		wr_data(*s++);
		col += 1;
		n += 1;
	}
	if (n & 1) {
		wr_data(GLCD_SPACE);
	}
}

void glcd_text_clear(void) {
	wr_command(cmdClear);
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
}

//-----------------------------------------------------------------------------
