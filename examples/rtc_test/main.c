//-----------------------------------------------------------------------------
/*

RTC Test Code

*/
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "hw.h"
#include "lcd.h"
#include "menu.h"
#include "keypad.h"
#include "delay.h"
#include "rtc.h"
#include "memshow.h"

//-----------------------------------------------------------------------------

static void get_time(struct menu *m) {
	uint8_t secs = 0xff;
	uint8_t dow = 0xff;
	lcd_clear();
	lcd_putc(m->rows - 1, 0, LEFT_ARROW);
	while (!key_exit()) {
		if (secs != rtc_get_secs()) {
			char tmp[32];
			struct rtc_time t;
			rtc_get_time(&t);
			secs = t.second;
			// these are fixed width fields, so just overwrite them
			lcd_puts(0, 0, rtc_hms(&t, tmp));
			lcd_puts(1, 0, rtc_date(&t, tmp));
			// day of week changes requires a row clear
			if (dow != t.day_of_week) {
				dow = t.day_of_week;
				lcd_clear_row(2);
				lcd_puts(2, 0, rtc_day_of_week(&t));
			}
		}
		delay_ms(100);
	}
}

//-----------------------------------------------------------------------------

static void get_ram(struct menu *m) {
	uint8_t buf[RTC_RAM_SIZE];
	rtc_get_ram(buf, sizeof(buf));

	struct memshow ms = {
		.rows = m->rows,
		.cols = m->cols,
		.buf = buf,
		.n = sizeof(buf),
		.adr = 0,
		.idx = 0,
	};

	memshow_run(&ms);
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "ds1302 rtc test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"get time", get_time},
	{"get ram", get_ram},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();

	if (!rtc_init()) {
		lcd_clear();
		lcd_puts(0, 0, "rtc not present");
		while (!key_exit()) ;
		return -1;
	}

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
