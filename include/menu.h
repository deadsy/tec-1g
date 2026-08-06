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

//void menu_down(struct menu *m);
//void menu_up(struct menu *m);
void menu_forward(struct menu *m, struct menu_item *items);
//void menu_back(struct menu *m);

void menu_init(struct menu *m, uint8_t rows, uint8_t cols, struct menu_item *items);
void menu_run(struct menu *m);

//-----------------------------------------------------------------------------

#endif				// MENU_H

//-----------------------------------------------------------------------------
