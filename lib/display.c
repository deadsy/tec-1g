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

void display_clear(void) {
	for (int8_t i = 0; i < NUM_CHARS; i++) {
		display[i] = 0;
	}
	digitPort = 0;
	segmentPort = 0;
}

//-----------------------------------------------------------------------------

// render a decimal number in a field, 5..0, start >= end
static void render_dec(uint16_t val, uint8_t start, uint8_t end, uint8_t fill) {
	// display the decimal
	uint8_t i = end;
	do {
		display[i] = hex2segment[val % 10];
		val /= 10;
		i++;
	} while ((val != 0) && (i <= start));
	// fill the remaining field
	while (i <= start) {
		display[i] = fill;
		i++;
	}
}

// display a decimal number on the bottom 2 displays
void display_dec_lo(uint8_t val, uint8_t fill) {
	render_dec(val, 1, 0, fill);
}

// display a decimal number on the top 4 displays
void display_dec_hi(uint16_t val, uint8_t fill) {
	render_dec(val, 5, 2, fill);
}

// display a decimal number on all 6 displays
void display_dec(uint16_t val, uint8_t fill) {
	render_dec(val, 5, 0, fill);
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
