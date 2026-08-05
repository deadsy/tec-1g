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

void display_scan(void);
void display_off(void);
void display_init(void);
void display_set(uint8_t x, uint8_t val);
void display_byte(uint8_t val);
void display_address(uint16_t val);

//-----------------------------------------------------------------------------

#endif				// DISPLAY_H

//-----------------------------------------------------------------------------
