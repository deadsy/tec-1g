//-----------------------------------------------------------------------------
/*

Array88 Driver

Note:

The bottom left of the display is (0,0)
The top right of the display is (7,7)
(x, y) == (col, row)

*/
//-----------------------------------------------------------------------------

#include "hw.h"
#include "array88.h"
#include "delay.h"

//-----------------------------------------------------------------------------

#define NUM_ROWS 8
#define NUM_COLS 8

// array state
static uint8_t red[NUM_ROWS];
static uint8_t green[NUM_ROWS];
static uint8_t blue[NUM_ROWS];

//-----------------------------------------------------------------------------

// blank (turn off) the 8x8 display
void array88_blank(void) {
	xr88Port = 0;
	xg88Port = 0;
	xb88Port = 0;
	y88Port = 0;
}

// run a single mux cycle
void array88_mux(void) {
	static uint8_t i;
	i = (i + 1) & (NUM_COLS - 1);
	y88Port = 0;
	xr88Port = red[i];
	xg88Port = green[i];
	xb88Port = blue[i];
	y88Port = 1 << i;
}

// run some scan cycles
void array88_scan(uint8_t cycles) {
	while (cycles > 0) {
		for (int8_t i = 0; i < 8; i++) {
			array88_mux();
			delay_1ms();
		}
		cycles--;
	}
}

// initialise the 8x8 state
void array88_init(void) {
	array88_fill(BLACK);
	array88_blank();
}

//-----------------------------------------------------------------------------

void array88_fill(uint8_t color) {
	uint8_t r = (color & RED) ? 0xff : 0;
	uint8_t g = (color & GREEN) ? 0xff : 0;
	uint8_t b = (color & BLUE) ? 0xff : 0;
	for (int8_t i = 0; i < NUM_ROWS; i++) {
		red[i] = r;
		green[i] = g;
		blue[i] = b;
	}
}

// shift the array pixels left
void array88_shift_left(void) {
	for (int8_t i = 0; i < NUM_ROWS; i++) {
		red[i] <<= 1;
		green[i] <<= 1;
		blue[i] <<= 1;
	};
}

// shift the array pixels right
void array88_shift_right(void) {
	for (int8_t i = 0; i < NUM_ROWS; i++) {
		red[i] >>= 1;
		green[i] >>= 1;
		blue[i] >>= 1;
	};
}

// shift the array pixels up
void array88_shift_up(void) {
	for (int8_t i = 0; i < NUM_ROWS - 1; i++) {
		red[i] = red[i + 1];
		green[i] = green[i + 1];
		blue[i] = blue[i + 1];
	}
	red[NUM_ROWS - 1] = 0;
	green[NUM_ROWS - 1] = 0;
	blue[NUM_ROWS - 1] = 0;
}

// shift the array pixels down
void array88_shift_down(void) {
	for (int8_t i = NUM_ROWS - 1; i > 0; i--) {
		red[i] = red[i - 1];
		green[i] = green[i - 1];
		blue[i] = blue[i - 1];
	}
	red[0] = 0;
	green[0] = 0;
	blue[0] = 0;
}

// set an x,y pixel to a color
void array88_plot(uint8_t x, uint8_t y, uint8_t color) {
	x &= (NUM_COLS - 1);
	y &= (NUM_ROWS - 1);

	// reverse to give 0,0 in the bottom left corner
	x = NUM_COLS - 1 - x;
	y = NUM_ROWS - 1 - y;

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

// draw a horizontal line x0,y .. x1,y
void array88_hline(uint8_t x0, uint8_t x1, uint8_t y, uint8_t color) {
	uint8_t x = x0;
	while (x <= x1) {
		array88_plot(x, y, color);
		x++;
	}
}

// draw a vertical line x,y0 .. x,y1
void array88_vline(uint8_t y0, uint8_t y1, uint8_t x, uint8_t color) {
	uint8_t y = y0;
	while (y <= y1) {
		array88_plot(x, y, color);
		y++;
	}
}

// draw a box
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

//-----------------------------------------------------------------------------
