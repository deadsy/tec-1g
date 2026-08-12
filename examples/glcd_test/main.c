//-----------------------------------------------------------------------------
/*

Graphics LCD Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>

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

	glcd_puts(0,0, "00", false);
	glcd_puts(1,0, "10", false);
	glcd_puts(2,0, "20", false);
	glcd_puts(3,0, "30", false);

	glcd_puts(0,14, "0f", false);
	glcd_puts(1,14, "1f", false);
	glcd_puts(2,14, "2f", false);
	glcd_puts(3,14, "3f", false);

	glcd_puts(0,6, "8x16", false);
	glcd_puts(1,6, "Text", false);
	glcd_puts(2,3, "Characters", false);

	glcd_putc(3,6, '&', false);
	glcd_putc(3,7, '%', false);
	glcd_putc(3,8, '#', false);
	glcd_putc(3,9, '@', false);

	glcd_flush_text();

	while (!key_exit());

	glcd_clear_text(true);
}

//-----------------------------------------------------------------------------

#define SCROLL_WIDTH 16

static void scroll_puts0(const char *s) {
	glcd_puts(0, 0, s, false);
}

static void scroll_puts1(const char *s) {
	glcd_puts(1, 0, s, false);
}

static void scroll_puts2(const char *s) {
	glcd_puts(2, 0, s, false);
}

static const char msg0[] = "They see me scrollin' they hatin'   ";
static const char msg1[] = "This is the string end, beautiful friend   ";
static const char msg2[] = "Greg! The stop bit!     ";

static void scroll_test(struct menu *m) {
	(void)m;

	char buf[SCROLL_WIDTH]; // reuse for each scroller

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
	glcd_putc(3,0, GLCD_LEFT_ARROW, false);

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

static void about(struct menu *m) {
	menu_about(m, "st7920 glcd test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"text", text_test},
	{"scroll", scroll_test},
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
