//-----------------------------------------------------------------------------
/*

Memory Show

Display and scroll through memory contents on the LCD.

*/
//-----------------------------------------------------------------------------

#ifndef MEMSHOW_H
#define MEMSHOW_H

//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stddef.h>

//-----------------------------------------------------------------------------

struct memshow {
	uint8_t rows;		// lcd rows
	uint8_t cols;		// lcd cols
	const uint8_t *buf;	// buffer to display
	size_t n;		// length of buffer
	uint16_t adr;		// starting address of buffer
	uint16_t idx;		// current position
};

//-----------------------------------------------------------------------------

void memshow_run(struct memshow *m);

//-----------------------------------------------------------------------------

#endif				// MEMSHOW_H

//-----------------------------------------------------------------------------
