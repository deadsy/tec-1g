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
		array88_scan(20);
		c += 1;
	}
	array88_init();
}

//-----------------------------------------------------------------------------

static void vline_test(struct menu *m) {
	(void)m;
	uint8_t c = 0;
	while (!key_exit()) {
		for (uint8_t i = 0; i < 8; i++) {
			array88_vline(0, i, i, i + c);
		}
		array88_scan(20);
		c += 1;
	}
	array88_init();
}

//-----------------------------------------------------------------------------

static void hline_test(struct menu *m) {
	(void)m;
	uint8_t c = 0;
	while (!key_exit()) {
		for (uint8_t i = 0; i < 8; i++) {
			array88_hline(0, i, i, i + c);
		}
		array88_scan(20);
		c += 1;
	}
	array88_init();
}

//-----------------------------------------------------------------------------

static void random_test(struct menu *m) {
	(void)m;
	while (!key_exit()) {
		for (uint8_t i = 0; i < 16; i++) {
			int k = rand();
			array88_plot(k & 7, (k >> 3) & 7, (k >> 6) & 7);
		}
		array88_scan(20);
	}
	array88_init();
}

//-----------------------------------------------------------------------------

static void fill_random(void) {
	// random colors..
	for (uint8_t i = 0; i < 255; i++) {
		int k = rand();
		array88_plot(k & 7, (k >> 3) & 7, (k >> 6) & 7);
	}
}

static void shift_test(void (*shift_func)(void)) {
	fill_random();
	for (uint8_t i = 0; i < 8; i++) {
		array88_scan(40);
		shift_func();
	}
	array88_init();
}

static void shift_left_test(struct menu *m) {
	(void)m;
	shift_test(array88_shift_left);
}

static void shift_right_test(struct menu *m) {
	(void)m;
	shift_test(array88_shift_right);
}

static void shift_up_test(struct menu *m) {
	(void)m;
	shift_test(array88_shift_up);
}

static void shift_down_test(struct menu *m) {
	(void)m;
	shift_test(array88_shift_down);
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "8x8 rgb array test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"vline", vline_test},
	{"hline", hline_test},
	{"box", box_test},
	{"random", random_test},
	{"shift left", shift_left_test},
	{"shift right", shift_right_test},
	{"shift up", shift_up_test},
	{"shift down", shift_down_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();
	array88_init();

	srand(0xcafe);

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
