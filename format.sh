#!/bin/bash

FILES="
./examples/array88/main.c
./examples/display/main.c
./examples/lcd_test/main.c
./examples/menu_test/main.c
./examples/rtc_test/main.c
./examples/glcd_test/main.c
./include/array88.h
./include/delay.h
./include/display.h
./include/lcd.h
./include/scroll.h
./include/hw.h
./include/menu.h
./include/keypad.h
./include/rtc.h
./include/glcd.h
./include/memshow.h
./include/fat.h
./lib/array88.c
./lib/delay.c
./lib/display.c
./lib/lcd.c
./lib/scroll.c
./lib/menu.c
./lib/keypad.c
./lib/rtc.c
./lib/glcd.c
./lib/memshow.c
./lib/fat.c
"

for f in $FILES; do
  indent $f -brf -linux -l10000
	rm $f~
done

