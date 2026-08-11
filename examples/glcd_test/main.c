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

//-----------------------------------------------------------------------------

static void text_test(struct menu *m) {
	(void)m;
	glcd_puts(0,0, "00");
	glcd_puts(1,0, "10");
	glcd_puts(2,0, "20");
	glcd_puts(3,0, "30");

	glcd_puts(0,15, "0f");
	glcd_puts(1,15, "1f");
	glcd_puts(2,15, "2f");
	glcd_puts(3,15, "3f");

	while (!key_exit());

	glcd_text_clear();
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "st7920 glcd test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"text", text_test},
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
