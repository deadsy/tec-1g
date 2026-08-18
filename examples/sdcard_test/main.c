//-----------------------------------------------------------------------------
/*

SDCARD/FAT32 Test Code

*/
//-----------------------------------------------------------------------------

//#include <string.h>
#include <stdio.h>

#include "lcd.h"
#include "menu.h"
#include "keypad.h"
#include "sdcard.h"

//-----------------------------------------------------------------------------

static void dir_test(struct menu *m) {
	(void)m;

	while (!key_exit()) ;
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "sdcard test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"dir", dir_test},
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

	int8_t rc = sd_init();
	if (rc != 0) {
		char msg[32];
		sprintf(msg, "sd_init() %d", rc);
		menu_error(&m, msg, NULL);
		return 0;
	}

	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
