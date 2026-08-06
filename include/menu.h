//-----------------------------------------------------------------------------
/*

Menu System

*/
//-----------------------------------------------------------------------------

#ifndef MENU_H
#define MENU_H

//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------

struct menu_item {
	const char *name;	// menu item name
	void (*func)(void);	// menu function
};

#define MENU_LENGTH(x) (sizeof(x)/sizeof(struct menu_item))

// end of menu list marker
#define MENU_EOL {NULL, NULL}

struct menu_list {
	const struct menu_item *items;
	uint8_t last;		// last item in menu list
	uint8_t index;		// index of indicated menu item
	uint8_t start;		// index of first displayed row
	uint8_t end;		// index of last displayed row
};


struct menu {
	struct menu_list *list; // current menu list
	void (*clear)(void);	// clear the display
	void (*puts)(uint8_t row, uint8_t col, const char *s);	// put string to display
	uint8_t rows;		// display rows
	uint8_t cols;		// display cols
};

//-----------------------------------------------------------------------------

uint8_t menu_length(const struct menu_item *menu);

void menu_down(struct menu *m);
void menu_up(struct menu *m);
void menu_render(struct menu *m);
void menu_init(void);

//-----------------------------------------------------------------------------

#endif // MENU_H

//-----------------------------------------------------------------------------
