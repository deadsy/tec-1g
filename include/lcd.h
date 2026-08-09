//-----------------------------------------------------------------------------
/*

Character LCD Driver (hd44780)

*/
//-----------------------------------------------------------------------------

#ifndef LCD_H
#define LCD_H

//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------

#define LCD_ROWS 4
#define LCD_COLS 20

#define RIGHT_ARROW 0x7e
#define LEFT_ARROW 0x7f

//-----------------------------------------------------------------------------

void lcd_init(void);
void lcd_display_ctrl(bool on);
void lcd_cursor_ctrl(bool on, bool blink);
void lcd_clear(void);
void lcd_clear_row(uint8_t row);
void lcd_bitmap(uint8_t code, uint8_t * bitmap);
void lcd_putc(uint8_t row, uint8_t col, char c);
void lcd_puts(uint8_t row, uint8_t col, const char *s);

//-----------------------------------------------------------------------------

#endif				// LCD_H

//-----------------------------------------------------------------------------
