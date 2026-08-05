//-----------------------------------------------------------------------------
/*

Character LCD Driver (hd44780)

*/
//-----------------------------------------------------------------------------

#include "hw.h"
#include "lcd.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define MAX_ROWS 4
#define MAX_COLS 20

//-----------------------------------------------------------------------------
// hd44780 commands

#define	cmdClear 0x01
#define	cmdHome 0x02
#define	cmdEntryMode 0x04
#define	cmdDisplay 0x08
#define	cmdShift 0x10
#define	cmdFunction 0x20
#define	cmdSetCgramAddr 0x40
#define	cmdSetDramAddr 0x80

#define BUSYFLAG 0x80

// cmdFunction
#define DATA8 (1 << 4 /*DL*/)
#define DATA4 (0 << 4 /*DL*/)
#define NL1 (0 << 3 /*N*/)
#define NL2 (1 << 3 /*N*/)
#define FONT(x) ((x) << 2 /*F*/)

// cmdDisplay
#define DISPLAY(x) ((x) << 2 /*D*/)
#define CURSOR(x) ((x) << 1 /*C*/)
#define BLINK(x) ((x) << 0 /*B*/)

// cmdEntryMode
#define INCDEC(x) ((x)<< 1 /*ID*/)
#define SHIFT(x) ((x)<< 0 /*S*/)

//-----------------------------------------------------------------------------

static bool display_state;	// on/off
static bool cursor_state;	// on/off
static bool blink_state;	// on/off

//-----------------------------------------------------------------------------

static void wr_command(uint8_t cmd) {
	lcdCmdPort = cmd;
	while ((lcdCmdPort & BUSYFLAG) != 0) ;
}

static void wr_data(uint8_t val) {
	lcdDataPort = val;
	while ((lcdCmdPort & BUSYFLAG) != 0) ;
}

//-----------------------------------------------------------------------------

static const uint8_t row2address[4] = { 0, 0x40, 0x14, 0x54 };

static void cursor_set(uint8_t row, uint8_t col) {
	uint8_t addr = (row2address[row & 3] + col) & 0x7f;
	wr_command(cmdSetDramAddr | addr);
}

//-----------------------------------------------------------------------------

static inline bool valid_posn(uint8_t row, uint8_t col) {
	return (row < MAX_ROWS) && (col < MAX_COLS);
}

// display a string at the row/col location.
void lcd_string(uint8_t row, uint8_t col, const char *s) {
	if (!valid_posn(row, col)) {
		return;
	}
	cursor_set(row, col);
	while ((*s != 0) && (col < MAX_COLS)) {
		wr_data(*s);
		s++;
		col++;
	}
}

// display a character at the row/col location.
void lcd_char(uint8_t row, uint8_t col, char c) {
	if (!valid_posn(row, col)) {
		return;
	}
	cursor_set(row, col);
	wr_data(c);
}

//-----------------------------------------------------------------------------

// display control - on/off.
void lcd_display_ctrl(bool on) {
	display_state = on;
	wr_command(cmdDisplay | DISPLAY(display_state & 1) | CURSOR(cursor_state & 1) | BLINK(blink_state & 1));
}

// cursor control - on/off and blinking.
void lcd_cursor_ctrl(bool on, bool blink) {
	cursor_state = on;
	blink_state = blink;
	wr_command(cmdDisplay | DISPLAY(display_state & 1) | CURSOR(cursor_state & 1) | BLINK(blink_state & 1));
}

//-----------------------------------------------------------------------------

// initialise the display
void lcd_init(void) {
	delay_ms(20);
	lcdCmdPort = cmdFunction | DATA8;
	delay_ms(5);
	lcdCmdPort = cmdFunction | DATA8;
	delay_1ms();
	lcdCmdPort = cmdFunction | DATA8;
	delay_1ms();
	wr_command(cmdFunction | DATA8 | NL2 | FONT(0));
	wr_command(cmdDisplay | DISPLAY(0) | CURSOR(0) | BLINK(0));
	wr_command(cmdClear);
	wr_command(cmdEntryMode | INCDEC(1) | SHIFT(0));

	display_state = false;
	cursor_state = false;
	blink_state = false;
}

//-----------------------------------------------------------------------------
