//-----------------------------------------------------------------------------
/*

Array88 Driver

*/
//-----------------------------------------------------------------------------

#include "hw.h"
#include "array88.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define NUM_ROWS 8
#define NUM_COLS 8

static uint8_t red[NUM_ROWS];
static uint8_t green[NUM_ROWS];
static uint8_t blue[NUM_ROWS];

//-----------------------------------------------------------------------------

void array88_off(void) {
	for (int8_t i = 0; i < NUM_ROWS; i++) {
		red[i] = 0;
		green[i] = 0;
		blue[i] = 0;
	}
}

void array88_scan(void) {
	for (int8_t i = 0; i < 8; i++) {
		xr88Port = red[i];
		xg88Port = green[i];
		xb88Port = blue[i];
		y88Port = 1 << i;
		delay_1ms();
		y88Port = 0;
	}
}

void array88_plot(uint8_t x, uint8_t y, uint8_t color) {
	x &= (NUM_COLS - 1);
	y &= (NUM_ROWS - 1);

	uint8_t xmask = 1 << x;

	if ((color & RED) != 0) {
		red[y] |= xmask;
	} else {
		red[y] &= ~xmask;
	}

	if ((color & GREEN) != 0) {
		green[y] |= xmask;
	} else {
		green[y] &= ~xmask;
	}

	if ((color & BLUE) != 0) {
		blue[y] |= xmask;
	} else {
		blue[y] &= ~xmask;
	}
}

void array88_hline(uint8_t x0, uint8_t x1, uint8_t y, uint8_t color) {
	uint8_t x = x0;
	while (x <= x1) {
		array88_plot(x, y, color);
		x++;
	}
}

void array88_vline(uint8_t y0, uint8_t y1, uint8_t x, uint8_t color) {
	uint8_t y = y0;
	while (y <= y1) {
		array88_plot(x, y, color);
		y++;
	}
}

void array88_box(uint8_t left, uint8_t bottom, uint8_t right, uint8_t top, uint8_t color) {
	if ((left == right) && (top == bottom)) {
		array88_plot(left, top, color);
		return;
	}
	if (left == right) {
		array88_vline(bottom, top, left, color);
		return;
	}
	if (bottom == top) {
		array88_hline(left, right, bottom, color);
		return;
	}
	array88_hline(left, right, bottom, color);
	array88_hline(left, right, top, color);
	array88_vline(bottom + 1, top - 1, left, color);
	array88_vline(bottom + 1, top - 1, right, color);
}

void array88_init(void) {
	array88_off();
	xr88Port = 0;
	xg88Port = 0;
	xb88Port = 0;
	y88Port = 0;
}

//-----------------------------------------------------------------------------
