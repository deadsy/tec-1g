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
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "A440");
	play_note(69, 50000);
}

//-----------------------------------------------------------------------------

#define NOTE_DURATION 5000
#define NOTE_BASE 60

static void cmajor_test(struct menu *m) {
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "C-Major Scale");
	play_note(NOTE_BASE + 0, NOTE_DURATION);
	play_note(NOTE_BASE + 2, NOTE_DURATION);
	play_note(NOTE_BASE + 4, NOTE_DURATION);
	play_note(NOTE_BASE + 5, NOTE_DURATION);
	play_note(NOTE_BASE + 7, NOTE_DURATION);
	play_note(NOTE_BASE + 9, NOTE_DURATION);
	play_note(NOTE_BASE + 11, NOTE_DURATION);
	play_note(NOTE_BASE + 12, NOTE_DURATION);
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "sound test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"a440", a440_test},
	{"c-major", cmajor_test},
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
