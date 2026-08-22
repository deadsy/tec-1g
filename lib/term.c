//-----------------------------------------------------------------------------
/*

GLCD Terminal

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "term.h"
#include "glcd.h"
#include "font.h"

//-----------------------------------------------------------------------------

// we use an 8x8 font on the GLCD, so we get..
#define TERM_ROWS (GLCD_PIXELS_V / 8)
#define TERM_COLS (GLCD_PIXELS_H / 8)
#define TERM_CHARS (TERM_ROWS * TERM_COLS)

//-----------------------------------------------------------------------------

// current row and column
static uint8_t termRow;
static uint8_t termCol;

// are we wrapping at the end of line?
static bool termWrap;

// display buffer
static uint8_t termBuffer[TERM_CHARS];

//-----------------------------------------------------------------------------

static void term_scroll_up(void) {
	if (termRow == TERM_ROWS - 1) {
		// copy rows 1..N to 0..N-1
		memcpy(termBuffer, &termBuffer[TERM_COLS], TERM_CHARS - TERM_COLS);
		// clear the last row
		memset(&termBuffer[TERM_CHARS - TERM_COLS], 0, TERM_COLS);
	} else {
		termRow++;
	}
	termCol = 0;
}

void term_putc(char c) {
	if (c == 0) {
		return;
	}
	if (c == '\n') {
		term_scroll_up();
	} else {
		if ((termCol == TERM_COLS) && termWrap) {
			term_scroll_up();
		}
		if (termCol < TERM_COLS) {
			termBuffer[(termRow * TERM_COLS) + termCol] = c;
			termCol++;
		}
	}
}

void term_puts(const char *s) {
	while (*s != 0) {
		term_putc(*s++);
	}
}

//-----------------------------------------------------------------------------

// flush the display buffer content to the glcd.
void term_flush(void) {
	for (uint8_t i = 0; i < TERM_CHARS; i++) {
		uint8_t x = (i & 15) << 3;	// 16 characters per row
		uint8_t y = (i >> 4) << 3;
		glcd_bmp88(x, y, get_glyph(termBuffer[i]));
	}
	glcd_flush();
}

//-----------------------------------------------------------------------------

void term_init(bool wrap) {
	memset(termBuffer, 0, sizeof(termBuffer));
	termWrap = wrap;
	termRow = 0;
	termCol = 0;
	glcd_clear(true);
}

//-----------------------------------------------------------------------------
