//-----------------------------------------------------------------------------
/*

SDCARD/FAT32 Test Code

*/
//-----------------------------------------------------------------------------

#include <stdio.h>

#include "lcd.h"
#include "menu.h"
#include "keypad.h"
#include "sdcard.h"
#include "fat.h"

#include "hw.h"

//-----------------------------------------------------------------------------

static Fat globalFat;
static Dir globalDir;
static DirInfo globalDirInfo;

#define mountPoint "mnt"
#define rootPath "/mnt"

//-----------------------------------------------------------------------------

static const DiskOps ops = {
	sd_read,
	sd_write,
};

//-----------------------------------------------------------------------------

static void dir_test(struct menu *m) {
	char msg[32];
	int rc;

	rc = fat_dir_open(&globalDir, rootPath);
	if (rc != 0) {
		sprintf(msg, "fat_dir_open() %d", rc);
		menu_error(m, msg, NULL);
		return;
	}

	lcd_clear();
	uint8_t row = 0;

	while (true) {
		rc = fat_dir_read(&globalDir, &globalDirInfo);
		if (rc != 0) {
			if (rc == FAT_ERR_EOF) {
				break;
			}
			sprintf(msg, "fat_dir_read() %d", rc);
			menu_error(m, msg, NULL);
			return;
		}

		lcd_puts(row, 0, globalDirInfo.name);
		testPort = globalDirInfo.name_len;
		row += 1;

		rc = fat_dir_next(&globalDir);
		if (rc != 0) {
			sprintf(msg, "fat_dir_next() %d", rc);
			menu_error(m, msg, NULL);
			return;
		}
	}

	while (!key_exit()) ;
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "sdcard test", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"dir", dir_test},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {
	int8_t rc;
	char msg[32];

	key_init();
	lcd_init();
	menu_init();

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);

	rc = sd_init();
	if (rc != 0) {
		sprintf(msg, "sd_init() %d", rc);
		menu_error(&m, msg, NULL);
		return 0;
	}

	rc = fat_probe(&ops, 0);
	if (rc != 0) {
		sprintf(msg, "fat_probe() %d", rc);
		menu_error(&m, msg, NULL);
		return 0;
	}

	rc = fat_mount(&ops, 0, &globalFat, mountPoint);
	if (rc != 0) {
		sprintf(msg, "fat_mount() %d", rc);
		menu_error(&m, msg, NULL);
		return 0;
	}

	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
