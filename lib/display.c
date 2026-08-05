//-----------------------------------------------------------------------------
/*

Display Driver for 6 x Seven Segment LED Display

*/
//-----------------------------------------------------------------------------

#include "hw.h"
#include "display.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define NUM_CHARS 6

static uint8_t display[NUM_CHARS];

static const uint8_t hex2segment[16] = {
	0xEB, 0x28, 0xCD, 0xAD, 0x2E, 0xA7, 0xE7, 0x29,	// 0-7
	0xEF, 0x2F, 0x6F, 0xE6, 0xC3, 0xEC, 0xC7, 0x47,	// 8-F
};

//-----------------------------------------------------------------------------

void display_scan(void) {
	for (int8_t i = 0; i < NUM_CHARS; i++) {
		segmentPort = display[i];
		digitPort = 1 << i;
		delay_1ms();
		digitPort = 0;
	}
}

void display_set(uint8_t x, uint8_t val) {
	if (x >= NUM_CHARS) {
		return;
	}
	display[x] = val;
}

void display_off(void) {
	for (int8_t i = 0; i < NUM_CHARS; i++) {
		display[i] = 0;
	}
}

void display_byte(uint8_t val) {
	display_set(0, hex2segment[val & 15]);
	display_set(1, hex2segment[val >> 4]);
}

void display_address(uint16_t val) {
	display_set(2, hex2segment[val & 15]);
	display_set(3, hex2segment[(val >> 4) & 15]);
	display_set(4, hex2segment[(val >> 8) & 15]);
	display_set(5, hex2segment[val >> 12]);
}

void display_init(void) {
	display_off();
	digitPort = 0;
	segmentPort = 0;
}

//-----------------------------------------------------------------------------
