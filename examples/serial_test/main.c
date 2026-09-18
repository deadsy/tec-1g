//-----------------------------------------------------------------------------
/*

Serial Port Test

*/
//-----------------------------------------------------------------------------

#include <stdio.h>

#include "serial.h"
#include "menu.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"

//-----------------------------------------------------------------------------

static void tx_test(struct menu *m) {
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "tx test");
	lcd_puts(1, 0, "running...");
	for (uint8_t i = 0; i < 100; i++) {
		serial_tx('A');
	}
	lcd_clear_row(1);
	lcd_puts(1, 0, "done");
	while (!key_exit()) ;
}

static void rx_test(struct menu *m) {
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "rx test");
	lcd_puts(1, 0, "running...");
	// TODO
	while (!key_exit()) ;
}

static void loopback_test(struct menu *m) {
	(void)m;
	lcd_clear();
	lcd_puts(0, 0, "loopback");
	lcd_puts(1, 0, "running...");
	// TODO
	while (!key_exit()) ;
}

//-----------------------------------------------------------------------------
// change the baud rate

static void set_baud_rate(uint16_t baud, uint16_t rate) {
	lcd_clear();
	char tmp[32];
	sprintf(tmp, "set to %d baud", baud);
	lcd_puts(0, 0, tmp);
	serial_init(rate);
	delay_500ms();
}

static void set_baud_4800(struct menu *m) {
	(void)m;
	set_baud_rate(4800, BAUD_4800);
}

static void set_baud_9600(struct menu *m) {
	(void)m;
	set_baud_rate(9600, BAUD_9600);
}

static void set_baud_19200(struct menu *m) {
	(void)m;
	set_baud_rate(19200, BAUD_19200);
}

static const struct menu_item baud_menu[] = {
	{"4800", set_baud_4800},
	{"9600", set_baud_9600},
	{"19200", set_baud_19200},
	MENU_EOL,
};

static void set_baud(struct menu *m) {
	struct menu sub;
	menu_setup(&sub, m->rows, m->cols, baud_menu);
	menu_run(&sub);
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "serial test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"baud", set_baud},
	{"tx", tx_test},
	{"rx", rx_test},
	{"loopback", loopback_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	key_init();
	lcd_init();
	menu_init();

	// default to 4800 baud
	serial_init(BAUD_4800);

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
