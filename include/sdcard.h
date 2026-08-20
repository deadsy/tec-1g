//-----------------------------------------------------------------------------
/*

SD Card Driver

*/
//-----------------------------------------------------------------------------

#ifndef SDCARD_H
#define SDCARD_H

//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// errors

#define SD_ERR_NO_CARD -1	// no card detected
#define SD_ERR_RESET_FAIL -2	// can't reset card
#define SD_ERR_TIMEOUT -3	// operation timeout
#define SD_ERR_VERSION_FAIL -4	// get card version failed
#define SD_ERR_OCR_FAIL -5	// get card ocr failed
#define SD_ERR_IDLE_FAIL -6	// the card is not idle
#define SD_ERR_BLOCK_LENGTH_FAIL -7	// can't set the block length (sdsc only)

//-----------------------------------------------------------------------------

int8_t sd_init(void);

// fat32 api
bool sd_read(uint8_t * buf, uint32_t sect);
bool sd_write(const uint8_t * buf, uint32_t sect);

//-----------------------------------------------------------------------------

#endif				// SDCARD_H

//-----------------------------------------------------------------------------
