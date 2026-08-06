//-----------------------------------------------------------------------------
/*

LCD Menu System

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "menu.h"
#include "lcd.h"
#include "keypad.h"

//-----------------------------------------------------------------------------
// marker characters

static const uint8_t up_arrow[] = { 0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00 };
static const uint8_t down_arrow[] = { 0x00, 0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04 };

#define UP_ARROW 0x05		// custom
#define DOWN_ARROW 0x06		// custom
#define SELECT_ARROW 0x7e	// standard

//-----------------------------------------------------------------------------

// return the number of items in a menu list
static uint8_t menu_length(const struct menu_item *menu) {
	uint8_t n = 0;
	while (menu->name != NULL) {
		menu++;
		n++;
	}
	return n;
}

//-----------------------------------------------------------------------------

// move down in the menu
static void menu_down(struct menu *m) {
	if (m->index < m->last) {
		if (m->index == m->end) {
			m->start += 1;
			m->end += 1;
		}
		m->index += 1;
	}
}

// move up in the menu
static void menu_up(struct menu *m) {
	if (m->index > 0) {
		if (m->index == m->start) {
			m->start -= 1;
			m->end -= 1;
		}
		m->index -= 1;
	}
}

//-----------------------------------------------------------------------------

// move forward in the menu
void menu_forward(struct menu *m, struct menu_item *items) {
	(void)m;
	(void)items;
}

// move backwards in the menu
static void menu_back(struct menu *m) {
	(void)m;
}

//-----------------------------------------------------------------------------

// render the curent state of the menu
static void menu_render(struct menu *m) {
	// clear the display
	lcd_clear();

	// render the menu items
	uint8_t row = 0;
	for (int i = m->start; i <= m->end; i++) {
		// current item marker
		if (i == m->index) {
			lcd_putc(row, 0, SELECT_ARROW);
		}
		lcd_puts(row, 1, m->items[i].name);
		row++;
	}

	// entries above marker
	if (m->start != 0) {
		lcd_putc(0, m->cols - 1, UP_ARROW);
	}

	// entries below marker
	if (m->end != m->last) {
		lcd_putc(m->rows - 1, m->cols - 1, DOWN_ARROW);
	}
}

//-----------------------------------------------------------------------------

void menu_run(struct menu *m) {
	menu_render(m);
	while (1) {
		if (!key_down()) {
			continue;
		}
		switch (key_code()) {
		case KEYPAD_Plus:{
				menu_down(m);
				break;
			}
		case KEYPAD_Minus:{
				menu_up(m);
				break;
			}
		default:{
				break;
			}
		}
		menu_render(m);
	}
}

//-----------------------------------------------------------------------------

// initialise the menu
void menu_init(struct menu *m, uint8_t rows, uint8_t cols, struct menu_item *items) {

	// setup the lcd
	lcd_bitmap(UP_ARROW, up_arrow);
	lcd_bitmap(DOWN_ARROW, down_arrow);

	// setup the menu
	m->prev = NULL;		// no previous menu
	m->items = items;
	m->rows = rows;
	m->cols = cols;
	m->last = menu_length(items) - 1;
	m->index = 0;
	m->start = 0;
	m->end = rows - 1;
}

//-----------------------------------------------------------------------------
