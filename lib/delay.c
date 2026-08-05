//-----------------------------------------------------------------------------
/*

Delay Routines

Assumes a clock rate of 4 MHz

*/
//-----------------------------------------------------------------------------

#include "delay.h"

//-----------------------------------------------------------------------------

/* *INDENT-OFF* */

void delay_125us(void) {
    __asm
        ld   b, #35         ; 7 T-states  : Load loop counter
    1$:
        djnz 1$             ; 450 T-states: (34 * 13) + 8 loop steps
        nop                 ; 4 T-states  : Fine-tuning padding
        nop                 ; 4 T-states  : Fine-tuning padding
        nop                 ; 4 T-states  : Fine-tuning padding
        push af             ; 11 T-states : High-cycle padding
        pop  af             ; 10 T-states : High-cycle padding
                            ; Note: SDCC automatically appends the 'ret' (10 T-states)
    __endasm;
}

/* *INDENT-ON* */

//-----------------------------------------------------------------------------

void delay_1ms(void) {
	delay_125us();
	delay_125us();
	delay_125us();
	delay_125us();
	delay_125us();
	delay_125us();
	delay_125us();
	delay_125us();
}

//-----------------------------------------------------------------------------

void delay_ms(uint8_t n) {
	while (n > 0) {
		delay_1ms();
		n--;
	}
}

//-----------------------------------------------------------------------------
