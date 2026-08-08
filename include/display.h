//-----------------------------------------------------------------------------
/*

Display Driver for 6 x Seven Segment LED Display

*/
//-----------------------------------------------------------------------------

#ifndef DISPLAY_H
#define DISPLAY_H

//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------
// fill characters

#define DISPLAY_SPACE 0
#define DISPLAY_ZERO 0xeb

//-----------------------------------------------------------------------------

void display_scan(void);
void display_clear(void);
void display_set(uint8_t x, uint8_t val);

// hexadecimal
void display_byte(uint8_t val);
void display_address(uint16_t val);

// decimal
void display_dec_lo(uint8_t val, uint8_t fill);
void display_dec_hi(uint16_t val, uint8_t fill);
void display_dec(uint16_t val, uint8_t fill);

//-----------------------------------------------------------------------------

#endif				// DISPLAY_H

//-----------------------------------------------------------------------------
