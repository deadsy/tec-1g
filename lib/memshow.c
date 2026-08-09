//-----------------------------------------------------------------------------
/*

Memory Show

Display and scroll through memory contents on the LCD.

*/
//-----------------------------------------------------------------------------

#include <stdio.h>

#include "memshow.h"
#include "lcd.h"
#include "menu.h"
#include "keypad.h"

//-----------------------------------------------------------------------------

#define BYTES_PER_ROW 4

//-----------------------------------------------------------------------------

static bool memshow_eol(struct memshow *ms) {
	uint16_t max = ((ms->n + BYTES_PER_ROW - 1) / BYTES_PER_ROW);
	return (ms->idx + ms->rows) >= max;
}

//-----------------------------------------------------------------------------

static void memshow_render_row(struct memshow *ms, uint8_t i, char *s) {
	uint16_t ofs = (ms->idx + i) * BYTES_PER_ROW;
	const uint8_t *ptr = &ms->buf[ofs];
	uint8_t k = MIN(BYTES_PER_ROW, ms->n - ofs);
	uint8_t n = 0;
	n += sprintf(&s[n], "%04x ", ms->adr + ofs);
	for (uint8_t i = 0; i < BYTES_PER_ROW; i++) {
		if (i < k) {
			n += sprintf(&s[n], "%02x ", ptr[i]);
		} else {
			n += sprintf(&s[n], "   ");
		}
	}
}

static void memshow_render(struct memshow *ms) {
	char tmp[LCD_COLS + 1];
	for (uint8_t i = 0; i < LCD_ROWS; i++) {
		memshow_render_row(ms, i, tmp);
		lcd_puts(i, 0, tmp);
	}
	// entries above marker
	uint8_t mark = (ms->idx != 0) ? UP_ARROW : LCD_SPACE;
	lcd_putc(0, ms->cols - 1, mark);
	// entries below marker
	mark = (!memshow_eol(ms)) ? DOWN_ARROW : LCD_SPACE;
	lcd_putc(ms->rows - 1, ms->cols - 1, mark);
}

//-----------------------------------------------------------------------------

void memshow_run(struct memshow *ms) {
	memshow_render(ms);
	bool render = false;
	while (1) {
		if (!key_down()) {
			continue;
		}
		switch (key_code()) {
		case KEYPAD_Plus:{
				if (!memshow_eol(ms)) {
					ms->idx += 1;
				}
				render = true;
				break;
			}
		case KEYPAD_Minus:{
				if (ms->idx != 0) {
					ms->idx -= 1;
				}
				render = true;
				break;
			}
		case KEYPAD_Address:{
				// return to previous menu
				return;
			}
		default:{
				break;
			}
		}
		if (render) {
			memshow_render(ms);
			render = false;
		}
	}
}

//-----------------------------------------------------------------------------
