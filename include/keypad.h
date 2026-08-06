//-----------------------------------------------------------------------------
/*

Keypad (74c923) Driver

*/
//-----------------------------------------------------------------------------

#ifndef KEYPAD_H
#define KEYPAD_H

//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------

#define KEYPAD_Shift (1 << 5)	// active low

#define KEYPAD_0       ( KEYPAD_Shift | 0x00)
#define KEYPAD_1       ( KEYPAD_Shift | 0x01)
#define KEYPAD_2       ( KEYPAD_Shift | 0x02)
#define KEYPAD_3       ( KEYPAD_Shift | 0x03)
#define KEYPAD_4       ( KEYPAD_Shift | 0x04)
#define KEYPAD_5       ( KEYPAD_Shift | 0x05)
#define KEYPAD_6       ( KEYPAD_Shift | 0x06)
#define KEYPAD_7       ( KEYPAD_Shift | 0x07)
#define KEYPAD_8       ( KEYPAD_Shift | 0x08)
#define KEYPAD_9       ( KEYPAD_Shift | 0x09)
#define KEYPAD_A       ( KEYPAD_Shift | 0x0a)
#define KEYPAD_B       ( KEYPAD_Shift | 0x0b)
#define KEYPAD_C       ( KEYPAD_Shift | 0x0c)
#define KEYPAD_D       ( KEYPAD_Shift | 0x0d)
#define KEYPAD_E       ( KEYPAD_Shift | 0x0e)
#define KEYPAD_F       ( KEYPAD_Shift | 0x0f)
#define KEYPAD_Plus    ( KEYPAD_Shift | 0x10)
#define KEYPAD_Minus   ( KEYPAD_Shift | 0x11)
#define KEYPAD_Go      ( KEYPAD_Shift | 0x12)
#define KEYPAD_Address ( KEYPAD_Shift | 0x13)
#define KEYPAD_None    ( 0xff)

//-----------------------------------------------------------------------------

bool key_down(void);
uint8_t key_code(void);

//-----------------------------------------------------------------------------

#endif				// KEYPAD_H

//-----------------------------------------------------------------------------
