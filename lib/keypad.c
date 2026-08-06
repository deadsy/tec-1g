//-----------------------------------------------------------------------------
/*

Keypad (74c923) Driver

*/
//-----------------------------------------------------------------------------

#include "hw.h"
#include "keypad.h"

//-----------------------------------------------------------------------------

#define KEYMASK 0x3f

//-----------------------------------------------------------------------------

static uint8_t current_code;

//-----------------------------------------------------------------------------

static inline bool shift_key(uint8_t code) {
	return (code & KEYPAD_Shift) == 0;
}

static uint8_t key_scan(void) {
	return keypadPort & KEYMASK;
}

bool key_down(void) {
	uint8_t code = key_scan();
	bool down = code != current_code;
	current_code = code;
	return down;
}

uint8_t key_code(void) {
	return current_code;
}

void key_init(void) {
	current_code = key_scan();
}

//-----------------------------------------------------------------------------
