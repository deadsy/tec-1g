//-----------------------------------------------------------------------------
/*

Sound Test Code

*/
//-----------------------------------------------------------------------------

#include "lcd.h"
#include "menu.h"
#include "keypad.h"
#include "sound.h"

//-----------------------------------------------------------------------------

static void a440_test(struct menu *m) {

	lcd_clear();
	lcd_puts(0, 0, "A440");
	lcd_putc(m->rows - 1, 0, LEFT_ARROW);

	while (!key_exit()) {
		play_note(69, 10000);
		//play_square(4000, 32768, 50000);
		//play_square(1000, 32768, 50000);
	}
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "sound test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"a440", a440_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
