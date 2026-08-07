//-----------------------------------------------------------------------------
/*

LCD Menu System

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "menu.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

//-----------------------------------------------------------------------------
// marker characters

static const uint8_t up_arrow[] = { 0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00 };
static const uint8_t down_arrow[] = { 0x00, 0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04 };
static const uint8_t ellipsis[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00 };

#define UP_ARROW 0x05		// custom
#define DOWN_ARROW 0x06		// custom
#define ELLIPSIS 0x07		// custom
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

// select a menu item
static void menu_select(struct menu *m) {
	// call the menu item function
	if (m->items[m->index].func != NULL) {
		m->items[m->index].func(m);
	} else {
		lcd_clear();
		lcd_puts(0, 0, "func == NULL");
		delay_500ms();
	}
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

// setup the menu with a menu item list
void menu_setup(struct menu *m, uint8_t rows, uint8_t cols, const struct menu_item *items) {
	uint8_t n = menu_length(items) - 1;	// watch out for empty menu item lists!
	m->items = items;
	m->rows = rows;
	m->cols = cols;
	m->last = n;
	m->index = 0;
	m->start = 0;
	m->end = MIN(rows - 1, n);
}

//-----------------------------------------------------------------------------

void menu_run(struct menu *m) {
	menu_render(m);
	bool render = false;
	while (1) {
		if (!key_down()) {
			continue;
		}
		switch (key_code()) {
		case KEYPAD_Plus:{
				menu_down(m);
				render = true;
				break;
			}
		case KEYPAD_Minus:{
				menu_up(m);
				render = true;
				break;
			}
		case KEYPAD_Go:{
				menu_select(m);
				render = true;
				break;
			}
		case KEYPAD_Address:{
				// return to previous menu
				return;
			}
		default:{
				break;
			}
		}
		if (render) {
			menu_render(m);
			render = false;
		}
	}
}

//-----------------------------------------------------------------------------

// initialise the menu
void menu_init(void) {
	// setup special lcd characters
	lcd_bitmap(UP_ARROW, up_arrow);
	lcd_bitmap(DOWN_ARROW, down_arrow);
	lcd_bitmap(ELLIPSIS, ellipsis);
}

//-----------------------------------------------------------------------------
