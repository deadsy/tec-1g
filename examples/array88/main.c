//-----------------------------------------------------------------------------
/*

Array88 Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>

#include "array88.h"
#include "menu.h"
#include "lcd.h"
#include "keypad.h"

//-----------------------------------------------------------------------------

static void box_test(struct menu *m) {
	(void)m;
	uint8_t c = 0;
	while (!key_exit()) {
		for (uint8_t i = 0; i < 4; i++) {
			array88_box(i, i, 7 - i, 7 - i, i + c);
		}
		for (uint8_t n = 0; n < 20; n++) {
			array88_scan();
		}
		c += 1;
	}
	array88_clear();
}

//-----------------------------------------------------------------------------

static void vline_test(struct menu *m) {
	(void)m;
	uint8_t c = 0;
	while (!key_exit()) {
		for (uint8_t i = 0; i < 8; i++) {
			array88_vline(0, i, i, i + c);
		}
		for (uint8_t n = 0; n < 20; n++) {
			array88_scan();
		}
		c += 1;
	}
	array88_clear();
}

//-----------------------------------------------------------------------------

static void hline_test(struct menu *m) {
	(void)m;
	uint8_t c = 0;
	while (!key_exit()) {
		for (uint8_t i = 0; i < 8; i++) {
			array88_hline(0, i, i, i + c);
		}
		for (uint8_t n = 0; n < 20; n++) {
			array88_scan();
		}
		c += 1;
	}
	array88_clear();
}

//-----------------------------------------------------------------------------

static void random_test(struct menu *m) {
	(void)m;
	srand(0xcafe);
	while (!key_exit()) {
		for (uint8_t i = 0; i < 16; i++) {
			int k = rand();
			array88_plot(k & 7, (k >> 3) & 7, (k >> 6) & 7);
		}
		for (uint8_t n = 0; n < 20; n++) {
			array88_scan();
		}
	}
	array88_clear();
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "8x8 rgb array test");
}

static const struct menu_item root_items[] = {
	{"vline", vline_test},
	{"hline", hline_test},
	{"box", box_test},
	{"random", random_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();
	array88_clear();

	lcd_display_ctrl(true);

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
