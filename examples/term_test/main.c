//-----------------------------------------------------------------------------
/*

GLCD Terminal Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>

#include "lcd.h"
#include "glcd.h"
#include "menu.h"
#include "keypad.h"
#include "term.h"

//-----------------------------------------------------------------------------

static void terminal_test(struct menu *m) {

	lcd_clear();
	lcd_puts(0, 0, "terminal");
	lcd_puts(1, 0, "addr/esc to exit");
	lcd_putc(m->rows - 1, 0, LEFT_ARROW);

	// cursor + wrap
	term_init(true, true);

	while (true) {
		if (key_down()) {
			uint8_t code = key_code();
			if (code == KEYPAD_Address) {
				glcd_clear(true);
				break;
			}
			term_putc(key_ascii(code));
			term_flush();
		}
	}
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "glcd terminal test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"terminal", terminal_test},
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
