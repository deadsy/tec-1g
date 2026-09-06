//-----------------------------------------------------------------------------
/*

Keypad (74c923) Driver

When a key is pressed the '923 data available line is asserted and the key
code is latched to the output of the '923. When the key is subsequently
released the data available line de-asserts, but the key code remains
latched.

*/
//-----------------------------------------------------------------------------

#include "hw.h"
#include "keypad.h"

//-----------------------------------------------------------------------------

#define KEYMASK 0x3f		// shift bit + 5 bit key code

//-----------------------------------------------------------------------------

static uint8_t current_code;
static bool prev_kda;

//-----------------------------------------------------------------------------

// return the shift key + the 74c923 key code.
static uint8_t key_scan(void) {
	return keypadPort & KEYMASK;
}

// return the 74c923 data available state (==keydown)
static bool key_data_available(void) {
	return (simpPort & simpKDA) == 0;
}

// is the shift key pressed for this key code?
static inline bool shift_key(uint8_t code) {
	return (code & KEYPAD_Shift) == 0;
}

//-----------------------------------------------------------------------------

// do we have a key down event?
bool key_down(void) {
	bool kda = key_data_available();
	// detect rising edge
	bool down = kda & !prev_kda;
	prev_kda = kda;
	if (down) {
		current_code = key_scan();
	}
	return down;
}

uint8_t key_code(void) {
	return current_code;
}

void key_init(void) {
	current_code = key_scan();
	prev_kda = key_data_available();
}

//-----------------------------------------------------------------------------

// return the ascii code for this key
uint8_t key_ascii(uint8_t code) {
	switch (code) {
	case KEYPAD_0:
		return '0';
	case KEYPAD_1:
		return '1';
	case KEYPAD_2:
		return '2';
	case KEYPAD_3:
		return '3';
	case KEYPAD_4:
		return '4';
	case KEYPAD_5:
		return '5';
	case KEYPAD_6:
		return '6';
	case KEYPAD_7:
		return '7';
	case KEYPAD_8:
		return '8';
	case KEYPAD_9:
		return '9';
	case KEYPAD_A:
		return 'A';
	case KEYPAD_B:
		return 'B';
	case KEYPAD_C:
		return 'C';
	case KEYPAD_D:
		return 'D';
	case KEYPAD_E:
		return 'E';
	case KEYPAD_F:
		return 'F';
	case KEYPAD_Plus:
		return '+';
	case KEYPAD_Minus:
		return '-';
	case KEYPAD_Go:
		return '\n';
	}
	return 0;
}

//-----------------------------------------------------------------------------

// return true if the "exit" key is pressed (address,minus)
bool key_exit(void) {
	if (!key_down()) {
		return false;
	}
	uint8_t code = key_code();
	return (code == KEYPAD_Address) || (code == KEYPAD_Minus);
}

//-----------------------------------------------------------------------------
