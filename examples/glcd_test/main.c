//-----------------------------------------------------------------------------
/*

Graphics LCD Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>

#include "lcd.h"
#include "menu.h"
#include "keypad.h"
#include "glcd.h"
#include "scroll.h"
#include "delay.h"

//-----------------------------------------------------------------------------

static void text_test(struct menu *m) {
	(void)m;

	glcd_clear_text(false);

	glcd_puts(0, 0, "00");
	glcd_puts(1, 0, "10");
	glcd_puts(2, 0, "20");
	glcd_puts(3, 0, "30");

	glcd_puts(0, 14, "0f");
	glcd_puts(1, 14, "1f");
	glcd_puts(2, 14, "2f");
	glcd_puts(3, 14, "3f");

	glcd_puts(0, 6, "8x16");
	glcd_puts(1, 6, "Text");
	glcd_puts(2, 3, "Characters");

	glcd_putc(3, 6, '&');
	glcd_putc(3, 7, '%');
	glcd_putc(3, 8, '#');
	glcd_putc(3, 9, '@');

	glcd_flush_text();

	while (!key_exit()) ;

	glcd_clear_text(true);
}

//-----------------------------------------------------------------------------

#define SCROLL_WIDTH 16

static void scroll_puts0(const char *s) {
	glcd_puts(0, 0, s);
}

static void scroll_puts1(const char *s) {
	glcd_puts(1, 0, s);
}

static void scroll_puts2(const char *s) {
	glcd_puts(2, 0, s);
}

static const char msg0[] = "They see me scrollin'.. They hatin'.. Wanna catch me codin' dirty...  ";
static const char msg1[] = "This is the string end.. beautiful friend  ";
static const char msg2[] = "Greg! The stop bit!  ";

static void scroll_test(struct menu *m) {
	(void)m;

	char buf[SCROLL_WIDTH];	// reuse for each scroller

	struct scroll s0 = {
		.msg = msg0,
		.len = strlen(msg0),
		.delta = +1,
		.width = SCROLL_WIDTH,
		.puts = scroll_puts0,
		.n = 0,
	};

	struct scroll s1 = {
		.msg = msg1,
		.len = strlen(msg1),
		.delta = -1,
		.width = SCROLL_WIDTH,
		.puts = scroll_puts1,
		.n = 0,
	};

	struct scroll s2 = {
		.msg = msg2,
		.len = strlen(msg2),
		.delta = +1,
		.width = SCROLL_WIDTH,
		.puts = scroll_puts2,
		.n = 0,
	};

	glcd_clear_text(false);
	glcd_putc(3, 0, GLCD_LEFT_ARROW);

	while (!key_exit()) {
		delay_ms(150);
		scroll_update(&s0, buf);
		scroll_update(&s1, buf);
		scroll_update(&s2, buf);
		glcd_flush_text();
	}

	glcd_clear_text(true);
}

//-----------------------------------------------------------------------------

static void hline_test(struct menu *m) {
	(void)m;

	for (uint8_t i = 0; i < 64; i++) {
		glcd_hline(0, i, i);
		glcd_hline(i + 64, 127, i);
		glcd_hline(i + 7, i + 9, i);
		glcd_hline(i + 15, i + 32, i);
	}
	glcd_flush_graphics();

	while (!key_exit()) ;
	glcd_clear_graphics(true);
}

//-----------------------------------------------------------------------------

static void vline_test(struct menu *m) {
	(void)m;

	glcd_clear(false);

	glcd_puts(0, 4, "Vertical");
	glcd_puts(1, 5, "Lines");

	for (uint8_t i = 0; i < 64; i++) {
		glcd_vline(0, i, i);
		glcd_vline(i + 5, i + 13, i);
		glcd_vline(0, i, 127 - i);
		glcd_vline(i + 5, i + 22, 127 - i);
	}
	glcd_flush();

	while (!key_exit()) ;
	glcd_clear(true);
}

//-----------------------------------------------------------------------------

#define BOX_SIZE 7
#define BOX_XSTEP 5
#define BOX_YSTEP 3

static void box_test(struct menu *m) {
	(void)m;

	uint8_t x0 = 0;
	uint8_t y0 = 0;
	uint8_t size = BOX_SIZE;

	glcd_clear(false);
	for (uint8_t i = 0; i < 15; i++) {
		glcd_box(x0, y0, x0 + size, y0 + size);
		x0 += BOX_XSTEP;
		y0 += BOX_YSTEP;
		size += 1;
	}
	glcd_flush();

	while (!key_exit()) ;
	glcd_clear(true);
}

//-----------------------------------------------------------------------------

// take care of boundary conditions for pixel values
static uint8_t get_pixel(int x, uint8_t y) {
	if (x < 0) {
		// left boundary condition
		return glcd_get_pixel(0, y);
	}
	if (x >= GLCD_PIXELS_H) {
		// right boundary condition
		return glcd_get_pixel(GLCD_PIXELS_H - 1, y);
	}
	return glcd_get_pixel(x, y);
}

static void ca_show(uint8_t rule) {
	// set the seed pixel
	glcd_set_pixel(63, 0);
	// generate the rows
	for (uint8_t y = 1; y < GLCD_PIXELS_V; y++) {
		// prime the pump for this row
		uint8_t n = (get_pixel(-1, y - 1) << 2) | (get_pixel(0, y - 1) << 1) | get_pixel(1, y - 1);
		for (uint8_t x = 0; x < GLCD_PIXELS_H; x++) {
			if (rule & (1 << n)) {
				glcd_set_pixel(x, y);
			}
			// next n
			n = ((n << 1) | get_pixel(x + 2, y - 1)) & 7;
		}
	}
}

static void ca_test(struct menu *m) {
	uint8_t rule = 30;
	while (1) {
		// show the rule on the lcd
		lcd_clear();
		char tmp[20];
		sprintf(tmp, "rule %d \x5\x6", rule);
		lcd_puts(0, 0, tmp);
		lcd_putc(m->rows - 1, 0, LEFT_ARROW);

		glcd_clear(false);
		ca_show(rule);
		glcd_flush();

		while (!key_down()) ;
		switch (key_code()) {
		case KEYPAD_Minus:{
				rule -= 1;
				break;
			}
		case KEYPAD_Plus:{
				rule += 1;
				break;
			}
		case KEYPAD_Address:{
				glcd_clear(true);
				return;
			}
		}
	}
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "st7920 glcd test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"text", text_test},
	{"scroll", scroll_test},
	{"hline", hline_test},
	{"vline", vline_test},
	{"box", box_test},
	{"1dca", ca_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();
	glcd_init();

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
