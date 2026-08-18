//-----------------------------------------------------------------------------
/*

SD Card Driver

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdbool.h>

#include "sdcard.h"
#include "hw.h"
#include "delay.h"

//-----------------------------------------------------------------------------
// command responses

#define rsp1Success  0		// success/ready
#define rsp1Idle  (1 << 0)	// in idle state
#define rsp1EraseReset  (1 << 1)	// erase reset
#define rsp1IllegalCommand  (1 << 2)	// illegal command
#define rsp1ComCRCError  (1 << 3)	// com crc error
#define rsp1EraseSequenceError  (1 << 4)	// erase sequence error
#define rsp1AddressError  (1 << 5)	// address error
#define rsp1ParameterError  (1 << 6)	// parameter error

//-----------------------------------------------------------------------------
// pre-canned commands

#define CMD_LEN 6

static const uint8_t cmd0[CMD_LEN] = { 0x40, 0, 0, 0, 0, 0x95 };	// reset, R1
static const uint8_t cmd8[CMD_LEN] = { 0x48, 0, 0, 1, 0xaa, 0x87 };	// send_if_cond, R7
static const uint8_t cmd16[CMD_LEN] = { 0x50, 0, 0, 2, 0, 1 };	// Set sector size to 512 bytes, R1
static const uint8_t cmd17[CMD_LEN] = { 0x51, 0, 0, 0, 0, 1 };	// read single block, R1
static const uint8_t cmd24[CMD_LEN] = { 0x58, 0, 0, 0, 0, 1 };	// write single block, R1
static const uint8_t cmd55[CMD_LEN] = { 0x77, 0, 0, 0, 0, 1 };	// APP_CMD, R1
static const uint8_t cmd58[CMD_LEN] = { 0x7A, 0, 0, 0, 0, 1 };	// READ_OCR, R3
static const uint8_t acmd41[CMD_LEN] = { 0x69, 0x40, 0, 0, 0, 1 };	// send_OP_COND, R1

//-----------------------------------------------------------------------------
// SPI bus operations

static uint8_t sdPortShadow;

// select the spi device (control the chip select line)
static void spi_select(bool state) {
	if (state) {
		// selected, cs is low
		sdPortShadow &= ~sdSelectMask;
	} else {
		// not selected, cs is high
		sdPortShadow |= sdSelectMask;
	}
	sdCardPort = sdPortShadow;
}

// transfer n bytes in/out of the spi interface
static void spi_xfer(uint8_t *rx, const uint8_t *tx, uint8_t n) {
	// assume CPOL = 0, CPHA = 0
	for (uint8_t i = 0; i < n; i++) {
		uint8_t rx_byte = 0;
		uint8_t tx_byte = 0xff;
		if (tx != NULL) {
			tx_byte = tx[i];
		}
		for (uint8_t j = 0; j < 8; j++) {
			// CPHA = 0, MOSI setup before rising edge
			// write MOSI (d7 thru d0)
			if (tx_byte & 0x80) {
				sdPortShadow |= sdMosiMask;
			} else {
				sdPortShadow &= ~sdMosiMask;
			}
			sdCardPort = sdPortShadow;
			tx_byte <<= 1;
			// CPOL = 0, clock idles low
			// clock high
			sdCardPort = sdPortShadow | sdClockMask;
			// clock low
			sdCardPort = sdPortShadow;
			// CPHA = 0, MISO read after falling edge
			// read MISO (d7 thru d0)
			uint8_t miso = (sdCardPort & sdMisoMask) ? 1 : 0;
			rx_byte = (rx_byte << 1) | miso;
		}
		if (rx != NULL) {
			rx[i] = rx_byte;
		}
	}
}

// tx n bytes to the spi device
static inline void spi_tx(const uint8_t *tx, uint8_t n) {
	spi_xfer(NULL, tx, n);
}

// rx n bytes from the spi device
static inline void spi_rx(uint8_t *rx, uint8_t n) {
	spi_xfer(rx, NULL, n);
}

static inline void spi_tx_byte(uint8_t val) {
	spi_tx(&val, 1);
}

//-----------------------------------------------------------------------------

// return true if the sd card is detected
static bool sd_detect(void) {
	return (sdCardPort & sdDetectMask) != 0;
}

static void sd_command(const uint8_t *cmd, uint8_t *rsp, uint8_t n) {
	spi_select(true);
	spi_tx(cmd, CMD_LEN);
	spi_rx(rsp, n);
	spi_select(false);
	// 8 clock cycles to help the sd card reset command state
	//spi_tx_byte(0xff);
}

//-----------------------------------------------------------------------------

int8_t sd_init(void) {

	if (!sd_detect()) {
		return SD_ERR_NO_CARD;
	}

	sdPortShadow = sdMosiMask;	// MOSI idles high
	spi_select(false);

	// Get the sd-card into a good initial state.
	// Run 80 spi clock cycles (while not selected).
	for (uint8_t i = 0; i < 10; i++) {
		spi_tx_byte(0xff);
	}

	// reset the card
	bool reset = false;
	for (uint8_t i = 0; i < 10; i++) {
		uint8_t rsp;
		sd_command(cmd0, &rsp, 1);

		if (rsp == rsp1Idle) {
			reset = true;
			break;
		}
		delay_ms(20);
	}
	if (!reset) {
		return SD_ERR_RESET_FAIL;
	}

	return 0;
}

//-----------------------------------------------------------------------------
