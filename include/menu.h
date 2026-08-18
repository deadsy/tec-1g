//-----------------------------------------------------------------------------
/*

LCD Menu System

*/
//-----------------------------------------------------------------------------

#ifndef MENU_H
#define MENU_H

//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------

#define UP_ARROW 0x05		// menu custom character
#define DOWN_ARROW 0x06		// menu custom character
#define ELLIPSIS 0x07		// menu custom character

// adds an ellipsis to indicate a sub-menu
#define ELLIPSIS_STR "\x7"
#define URL_PAD "  "

//-----------------------------------------------------------------------------

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

//-----------------------------------------------------------------------------

struct menu;			// forward declaration

struct menu_item {
	const char *name;	// menu item name
	void (*func)(struct menu * m);	// menu function
};

// end of menu list marker
#define MENU_EOL {NULL, NULL}

struct menu {
	struct menu *prev;	// previous menu level
	const struct menu_item *items;	// menu items
	uint8_t rows;		// display rows
	uint8_t cols;		// display cols
	uint8_t last;		// index of last item in menu list
	uint8_t index;		// index of indicated menu item
	uint8_t start;		// index of first displayed row
	uint8_t end;		// index of last displayed row
};

//-----------------------------------------------------------------------------

void menu_init(void);
void menu_setup(struct menu *m, uint8_t rows, uint8_t cols, const struct menu_item *items);
void menu_run(struct menu *m);
void menu_about(struct menu *m, const char *title, const char *url);
void menu_error(struct menu *m, const char *msg0, const char *msg1);

//-----------------------------------------------------------------------------

#endif				// MENU_H

//-----------------------------------------------------------------------------
