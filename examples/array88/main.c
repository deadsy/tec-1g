//-----------------------------------------------------------------------------
/*

Array88 Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "array88.h"
#include "menu.h"
#include "lcd.h"
#include "keypad.h"

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "8x8 rgb array test");
	lcd_puts(1, 0, "git:");
	lcd_puts(1, 5, GIT_HASH);
	while (!key_exit()) ;
}

//-----------------------------------------------------------------------------

static void box_test(struct menu *m) {
	(void)m;
	uint8_t c = 0;
	while (!key_exit()) {
		for (int8_t i = 0; i < 4; i++) {
			array88_box(i, i, 7 - i, 7 - i, i + c);
		}
		for (int n = 0; n < 20; n++) {
			array88_scan();
		}
		c += 1;
	}
}

//-----------------------------------------------------------------------------

static const struct menu_item root_items[] = {
	{"vline", NULL},
	{"hline", NULL},
	{"box", box_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();
	array88_init();

	lcd_display_ctrl(true);

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
