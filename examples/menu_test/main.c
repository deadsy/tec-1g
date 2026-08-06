//-----------------------------------------------------------------------------
/*

Menu Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "hw.h"
#include "lcd.h"
#include "menu.h"

//-----------------------------------------------------------------------------

static const struct menu_item menu0[] = {
	{"item 0.0", NULL},
	{"item 0.1", NULL},
	{"item 0.2", NULL},
	MENU_EOL,
};

static void sub_menu0(struct menu *m) {
	menu_forward(m, menu0);
}

//-----------------------------------------------------------------------------

static const struct menu_item menu8[] = {
	{"item 8.0", NULL},
	{"item 8.1", NULL},
	{"item 8.2", NULL},
	MENU_EOL,
};

static void sub_menu8(struct menu *m) {
	menu_forward(m, menu8);
}

//-----------------------------------------------------------------------------

static const struct menu_item root_items[] = {
	{"menu item 0", sub_menu0},
	{"menu item 1", NULL},
	{"menu item 2", NULL},
	{"menu item 3", NULL},
	{"menu item 4", NULL},
	{"menu item 5", NULL},
	{"menu item 6", NULL},
	{"menu item 7", NULL},
	{"menu item 8", sub_menu8},
	{"menu item 9", NULL},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {

	lcd_init();
	lcd_display_ctrl(true);

	struct menu m;
	menu_init(&m, 4, 20, root_items);
	menu_run(&m);

	return 0;
}

//-----------------------------------------------------------------------------
