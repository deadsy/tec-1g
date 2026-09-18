//-----------------------------------------------------------------------------
/*

Serial Driver

This is a very simple bit-banged serial driver.
The protocol is fixed to 8 data bits, no parity and 1 stop bit (8N1).
The typical baud rate is 4800, but other rates can be used.

*/
//-----------------------------------------------------------------------------

#include "serial.h"
#include "hw.h"

//-----------------------------------------------------------------------------

static uint16_t baud_rate;

// delay 1 serial bit time
static void serial_bit_delay(void) {
	uint16_t n = baud_rate;
	while (n > 0) {
		n--;
	}
}

//-----------------------------------------------------------------------------

// Note: we're not trying to preserve the value of other bits on digitPort.
// The port is write-only, so we can't do a RMW.

static inline void serial_lo(void) {
	digitPort = 0;
}

static inline void serial_hi(void) {
	digitPort = serialTxMask;
}

//-----------------------------------------------------------------------------

// tx a byte on the serial
void serial_tx(uint8_t c) {
	// start high
	serial_hi();
	// 1 start bit
	serial_lo();
	serial_bit_delay();
	// 8 data bits
	for (uint8_t i = 0; i < 8; i++) {
		// lsb-first
		if (c & 1) {
			serial_hi();
		} else {
			serial_lo();
		}
		c >>= 1;
		serial_bit_delay();
	}
	// 2 stop bits
	serial_lo();
	serial_bit_delay();
	serial_bit_delay();
	// end high
	serial_hi();
}

// rx a byte from the serial (return -1 on failure)
int8_t serial_rx(uint8_t *c, uint16_t timeout) {
	(void)c;
	(void)timeout;
	return 0;
}

void serial_init(uint16_t baud) {
	baud_rate = baud;
}

//-----------------------------------------------------------------------------

// put a string on the serial
void serial_puts(const char *s) {
	while (*s != 0) {
		serial_tx(*s++);
	}
}

// put a character on the serial
void serial_putc(char c) {
	serial_tx(c);
}

//-----------------------------------------------------------------------------
