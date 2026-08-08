//-----------------------------------------------------------------------------
/*

Display Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "menu.h"
#include "lcd.h"
#include "keypad.h"
#include "display.h"

//-----------------------------------------------------------------------------

static void hex_count_test(struct menu *m) {
	lcd_clear();
	lcd_puts(0, 0, "counting...");
	lcd_putc(m->rows - 1, 0, LEFT_ARROW);
	uint16_t a = 0;
	uint8_t b = 0;
	while (!key_exit()) {
		display_address(a);
		display_byte(b);
		for (uint8_t n = 0; n < 20; n++) {
			display_scan();
		}
		a += 1;
		b -= 1;
	}
	display_clear();
}

//-----------------------------------------------------------------------------

static void dec_count_test(struct menu *m) {
	lcd_clear();
	lcd_puts(0, 0, "counting...");
	lcd_putc(m->rows - 1, 0, LEFT_ARROW);
	uint16_t a = 0;
	uint8_t b = 0;
	while (!key_exit()) {
		display_dec_hi(a, DISPLAY_ZERO);
		display_dec_lo(b, DISPLAY_ZERO);
		for (uint8_t n = 0; n < 20; n++) {
			display_scan();
		}
		a += 1;
		b -= 1;
	}
	display_clear();
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "display test");
}

static const struct menu_item root_items[] = {
	{"hex counter", hex_count_test},
	{"dec counter", dec_count_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();
	display_clear();

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
