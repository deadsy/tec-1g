//-----------------------------------------------------------------------------
/*

SD Card Driver

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "sdcard.h"
#include "hw.h"
#include "delay.h"

//-----------------------------------------------------------------------------
// command responses

#define rsp1Success 0		// success/ready
#define rsp1Idle (1 << 0)	// in idle state
#define rsp1EraseReset (1 << 1)	// erase reset
#define rsp1IllegalCommand (1 << 2)	// illegal command
#define rsp1ComCRCError (1 << 3)	// com crc error
#define rsp1EraseSequenceError (1 << 4)	// erase sequence error
#define rsp1AddressError (1 << 5)	// address error
#define rsp1ParameterError (1 << 6)	// parameter error

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

// card is SDHC (high capacity), uses block addressing
static bool sdHighCapacity;

//-----------------------------------------------------------------------------
// SPI bus operations

static uint8_t sdPortShadow;

// select the spi device (control the chip select line)
static void spi_select(bool state) {
	if (state) {
		// cs=0, mosi=1, clk=0
		sdPortShadow = sdMosiMask;
	} else {
		// cs=1, mosi=1, clk=0
		sdPortShadow = sdSelectMask | sdMosiMask;
	}
	sdCardPort = sdPortShadow;
}

static inline void spi_clock_hi(void) {
	sdCardPort = sdPortShadow | sdClockMask;
}

static inline void spi_clock_lo(void) {
	sdCardPort = sdPortShadow;
}

static inline uint8_t sd_miso(void) {
	return (sdCardPort & sdMisoMask) ? 1 : 0;
}

// tx n bytes to the spi device
static inline void spi_tx(const uint8_t *tx, uint8_t n) {
	for (uint8_t i = 0; i < n; i++) {
		uint8_t tx_byte = tx[i];
		for (uint8_t j = 0; j < 8; j++) {
			// MOSI setup before rising edge
			// write MOSI (d7 thru d0)
			if (tx_byte & 0x80) {
				sdPortShadow |= sdMosiMask;
			} else {
				sdPortShadow &= ~sdMosiMask;
			}
			sdCardPort = sdPortShadow;
			tx_byte <<= 1;
			// cycle the clock
			spi_clock_hi();
			spi_clock_lo();
		}
	}
	// set MOSI=1
	sdPortShadow |= sdMosiMask;
}

// rx n bytes from the spi device
static inline void spi_rx(uint8_t *rx, uint8_t n) {
	// Note: The next MISO response bit was clocked out on the
	// last falling clock edge from the previous operation.
	for (uint8_t i = 0; i < n; i++) {
		uint8_t rx_byte = 0;
		for (uint8_t j = 0; j < 8; j++) {
			spi_clock_hi();
			rx_byte = (rx_byte << 1) | sd_miso();
			spi_clock_lo();
		}
		rx[i] = rx_byte;
	}
}

//-----------------------------------------------------------------------------

// return true if the sd card is detected
static bool sd_detect(void) {
	return (sdCardPort & sdDetectMask) != 0;
}

static int8_t sd_command(const uint8_t *cmd, uint8_t *rsp, uint8_t n) {
	int8_t rc = 0;
	spi_select(true);
	spi_tx(cmd, CMD_LEN);
	// get the response
	bool good = false;
	uint8_t val = 0;
	for (uint8_t i = 0; i < 10; i++) {
		spi_rx(&val, 1);
		if (val != 0xff) {
			good = true;
			break;
		}
	}
	if (good) {
		rsp[0] = val;
		// get the rest of the response bytes (if any)
		if (n > 1) {
			spi_rx(&rsp[1], n - 1);
		}
	} else {
		rc = SD_ERR_TIMEOUT;
	}
	spi_select(false);
	// 8 clock cycles to help the sd card go back to command state
	for (uint8_t i = 0; i < 8; i++) {
		spi_clock_hi();
		spi_clock_lo();
	}
	return rc;
}

//-----------------------------------------------------------------------------

int8_t sd_init(void) {
	uint8_t rsp[8];
	int8_t rc;

	if (!sd_detect()) {
		return SD_ERR_NO_CARD;
	}

	// Get the sd-card into a good initial state.
	// Run 80 spi clock cycles (while not selected).
	spi_select(false);
	for (uint8_t i = 0; i < 80; i++) {
		spi_clock_hi();
		spi_clock_lo();
	}

	// reset the card
	bool reset = false;
	for (uint8_t i = 0; i < 10; i++) {
		rc = sd_command(cmd0, rsp, 1);
		if (rc != 0) {
			return rc;
		}
		if (rsp[0] == rsp1Idle) {
			reset = true;
			break;
		}
		delay_ms(20);
	}
	if (!reset) {
		return SD_ERR_RESET_FAIL;
	}

	// get the card version
	rc = sd_command(cmd8, rsp, 5);
	if (rc != 0) {
		return rc;
	}
	if ((rsp[0] != rsp1Success) || (rsp[1] != 0) || (rsp[4] != 0xaa)) {
		return SD_ERR_VERSION_FAIL;
	}

	// check the card is idle
	bool idle = false;
	for (uint8_t i = 0; i < 10; i++) {
		rc = sd_command(cmd55, rsp, 1);
		if (rc != 0) {
			return rc;
		}
		rc = sd_command(acmd41, rsp, 1);
		if (rc != 0) {
			return rc;
		}
		if (rsp[0] == rsp1Success) {
			idle = true;
			break;
		}
		delay_ms(20);
	}
	if (!idle) {
		return SD_ERR_IDLE_FAIL;
	}

	// read the ocr (operation condition register)
	bool ocr = false;
	sdHighCapacity = false;
	for (uint8_t i = 0; i < 10; i++) {
		rc = sd_command(cmd58, rsp, 5);
		if (rc != 0) {
			return rc;
		}
		if ((rsp[0] == rsp1Success) && ((rsp[1] & 0x80) != 0)) {
			// card is not busy, sdhc or sdsc?
			sdHighCapacity = (rsp[1] & 0x40) != 0;
			ocr = true;
			break;
		}
		delay_ms(20);
	}
	if (!ocr) {
		return SD_ERR_OCR_FAIL;
	}

	// if we have SDSC, set the block length
	if (!sdHighCapacity) {
		rc = sd_command(cmd16, rsp, 1);
		if (rc != 0) {
			return rc;
		}
		if (rsp[0] != rsp1Success) {
			return SD_ERR_BLOCK_LENGTH_FAIL;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// fat32 api

bool sd_read(uint8_t *buf, uint32_t sect) {
	(void)buf;
	(void)sect;
	return false;
}

bool sd_write(const uint8_t *buf, uint32_t sect) {
	(void)buf;
	(void)sect;
	return false;
}

//-----------------------------------------------------------------------------
