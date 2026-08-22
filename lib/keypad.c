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

// return true if the "exit" key is pressed (escape,address,minus)
bool key_exit(void) {
	if (!key_down()) {
		return false;
	}
	uint8_t code = key_code();
	return (code == KEYPAD_Address) || (code == KEYPAD_Minus);
}

//-----------------------------------------------------------------------------
