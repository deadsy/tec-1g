#!/bin/bash

FILES="
./examples/array88/main.c
./examples/display/main.c
./examples/lcd_test/main.c
./examples/menu_test/main.c
./include/array88.h
./include/delay.h
./include/display.h
./include/lcd.h
./include/scroll.h
./include/hw.h
./lib/array88.c
./lib/delay.c
./lib/display.c
./lib/lcd.c
./lib/scroll.c
"

for f in $FILES; do
  indent $f -brf -linux -l10000
	rm $f~
done

