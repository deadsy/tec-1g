//-----------------------------------------------------------------------------
/*

LCD Test Code

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "lcd.h"
#include "scroll.h"

//-----------------------------------------------------------------------------

#define SCROLL_WIDTH 20

static const char msg[] = "This is a scrolled message!                 ";

static void scroll_puts(const char *s) {
	lcd_string(3, 0, s);
}

int main(void) {
	char buf[SCROLL_WIDTH];

	struct scroll s = {
		.msg = msg,
		.len = strlen(msg),
		.delta = +1,
		.width = SCROLL_WIDTH,
		.puts = scroll_puts,
		.n = 0,
	};

	lcd_init();
	lcd_display_ctrl(true);

	lcd_char(0, 0, 'A');
	lcd_char(1, 0, 'B');
	lcd_char(2, 0, 'C');

	lcd_char(0, 10, '0');
	lcd_char(1, 10, '1');
	lcd_char(2, 10, '2');

	scroll_output(&s, buf);

	int n = 0;
	while (1) {
		n++;
		if ((n & ((1 << 14) - 1)) == 0) {
			scroll_update(&s, buf);
		}
	}
}

//-----------------------------------------------------------------------------
