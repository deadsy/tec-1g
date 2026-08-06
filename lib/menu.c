//-----------------------------------------------------------------------------
/*

Menu System

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "menu.h"
#include "lcd.h"

//-----------------------------------------------------------------------------

static const uint8_t up_arrow[] = { 0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00 };
static const uint8_t down_arrow[] = { 0x00, 0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04 };

#define UP_ARROW 0x05
#define UP_ARROW_STR "\x05"

#define DOWN_ARROW 0x06
#define DOWN_ARROW_STR "\x06"

#define SELECT_ARROW_STR "\x7e"

//-----------------------------------------------------------------------------

uint8_t menu_length(const struct menu_item *menu) {
	uint8_t n = 0;
	while (menu->name != NULL) {
		menu++;
		n++;
	}
	return n;
}

// move down in the menu
void menu_down(struct menu *m) {
	struct menu_list *l = m->list;
	if (l->index < l->last) {
		if (l->index == l->end) {
			l->start += 1;
			l->end += 1;
		}
		l->index += 1;
	}
}

// move up in the menu
void menu_up(struct menu *m) {
	struct menu_list *l = m->list;
	if (l->index > 0) {
		if (l->index == l->start) {
			l->start -= 1;
			l->end -= 1;
		}
		l->index -= 1;
	}
}

//-----------------------------------------------------------------------------

// render the curent state of the menu
void menu_render(struct menu *m) {
	struct menu_list *l = m->list;
	// entries above marker
	if (l->start != 0) {
		m->puts(0, m->cols - 1, UP_ARROW_STR);
	}
	// entries below marker
	if (l->end != l->last) {
		m->puts(m->rows - 1, m->cols - 1, DOWN_ARROW_STR);
	}
	// render the menu items
	uint8_t row = 0;
	for (int i = l->start; i <= l->end; i++) {
		// current item marker
		if (i == l->index) {
			m->puts(row, 0, SELECT_ARROW_STR);
		}
		m->puts(row, 1, l->items[i].name);
		row++;
	}
}

//-----------------------------------------------------------------------------

// initialise the menu
void menu_init(void){
	lcd_bitmap(UP_ARROW, up_arrow);
	lcd_bitmap(DOWN_ARROW, down_arrow);
}

//-----------------------------------------------------------------------------
