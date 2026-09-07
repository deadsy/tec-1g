//-----------------------------------------------------------------------------
/*

Sound Test Code

*/
//-----------------------------------------------------------------------------

#include "lcd.h"
#include "menu.h"
#include "keypad.h"
#include "sound.h"
#include "mml.h"

//-----------------------------------------------------------------------------

static void a440_test(struct menu *m) {
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "440Hz");
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

// "Ode to Joy" (Beethoven) as an MML string.
static const char tune[] =
    "T200 O4 L4 "
    "E E F G G F E D C C D E E. D8 D2 "
    "E E F G G F E D C C D E D. C8 C2";

static void mml_test(struct menu *m) {
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "Ode to Joy");
	mml_play(tune);
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "sound test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"440Hz", a440_test},
	{"c-major", cmajor_test},
	{"mml", mml_test},
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
