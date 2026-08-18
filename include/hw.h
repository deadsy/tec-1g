//-----------------------------------------------------------------------------
/*

Hardware Defines

*/
//-----------------------------------------------------------------------------

#ifndef HW_H
#define HW_H

//-----------------------------------------------------------------------------
#if defined (HW_TEC_1G)

__sfr __at 0x00 keypadPort;	// keypad scan values
__sfr __at 0x01 digitPort;	// display digit enable
__sfr __at 0x02 segmentPort;	// display segment enable
__sfr __at 0x03 simpPort;	// General SIMP Input
__sfr __at 0x04 lcdCmdPort;	// LCD Display command
__sfr __at 0x05 y88Port;	// Standard 8x8 Row (Y) select
__sfr __at 0x06 xr88Port;	// RGB 8x8 (Red) column (X) select
__sfr __at 0x07 glcdCommandPort;	// GLCD command port
__sfr __at 0x84 lcdDataPort;	// LCD Display data
__sfr __at 0x87 glcdDataPort;	// GLCD data port
__sfr __at 0xf8 xg88Port;	// RGB 8x8 (Green) column (X) select
__sfr __at 0xf9 xb88Port;	// RGB 8x8 (Blue) column (X) select
__sfr __at 0xfc rtcPort;	// GPIO Real Time Clock
__sfr __at 0xfd sdCardPort;	// GPIO SD Card
__sfr __at 0xfe keyboardPort;	// Matrix Keyboard Input
__sfr __at 0xff systemPort;	// System Latch

// rtcPort
#define rtcOut (1 << 0)		// D0, output FROM rtc
#define rtcChipEnable (1 << 4)	// D4, active high
#define rtcWriteEnable (1 << 5)	// D5, active low
#define rtcClock (1 << 6)	// D6
#define rtcIn (1 << 7)		// D7, input TO rtc

// sdCardPort
#define sdMosiMask (1 << 0)	// D0 mosi (out)
#define sdClockMask (1 << 1)	// D1 clock (out)
#define sdSelectMask (1 << 2)	// D2 chip select (out, active low)
#define sdDetectMask (1 << 6)	// D6 card detect (in)
#define sdMisoMask (1 << 7)	// D7 miso (in)

//-----------------------------------------------------------------------------
#elif defined(HW_TEC_1)

// TODO

//-----------------------------------------------------------------------------

#else
#error "add #define constants for the new target"
#endif

//-----------------------------------------------------------------------------

#endif				// HW_H

//-----------------------------------------------------------------------------
