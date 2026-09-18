//-----------------------------------------------------------------------------
/*

Serial Port

*/
//-----------------------------------------------------------------------------

#ifndef SERIAL_H
#define SERIAL_H

//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------
// Not the literal baud rate - instead a delay tuned (verified by scope) for
// the baud rate that gives a 1 bit delay time (see serial_bit_delay).
// It's assumed the CPU is running at 4MHz.

#define BAUD_4800 100		// 4800 -> 1/4800 = 208.33uS
#define BAUD_9600 100		// 9600 -> 1/9600 = 104.17uS
#define BAUD_19200 100		// 19200 -> 1/19200 = 52.08uS

//-----------------------------------------------------------------------------

void serial_tx(uint8_t c);
int8_t serial_rx(uint8_t * c, uint16_t timeout);
void serial_init(uint16_t baud);

void serial_puts(const char *s);
void serial_putc(char c);

//-----------------------------------------------------------------------------

#endif				// SERIAL_H

//-----------------------------------------------------------------------------
