//-----------------------------------------------------------------------------
/*

Array88 Driver

*/
//-----------------------------------------------------------------------------

#ifndef ARRAY88_H
#define ARRAY88_H

//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------
// colors

#define BLACK 0
#define RED (1 << 0)
#define GREEN (1 << 1)
#define BLUE (1 << 2)
#define YELLOW (RED | GREEN)
#define MAGENTA (RED | BLUE)
#define CYAN (GREEN | BLUE)
#define WHITE (RED | GREEN | BLUE)

//-----------------------------------------------------------------------------

void array88_init(void);
void array88_off(void);
void array88_scan(void);
void array88_plot(uint8_t x, uint8_t y, uint8_t color);
void array88_hline(uint8_t x0, uint8_t x1, uint8_t y, uint8_t color);
void array88_vline(uint8_t y0, uint8_t y1, uint8_t x, uint8_t color);
void array88_box(uint8_t left, uint8_t bottom, uint8_t right, uint8_t top, uint8_t color);

//-----------------------------------------------------------------------------

#endif				// ARRAY88_H

//-----------------------------------------------------------------------------
