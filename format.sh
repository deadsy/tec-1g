#!/bin/bash

FILES="
./array88/main.c
./display/main.c
./lcd_test/main.c
./lib/array88.c
./lib/array88.h
./lib/delay.c
./lib/delay.h
./lib/display.c
./lib/display.h
./lib/lcd.c
./lib/lcd.h
./lib/scroll.c
./lib/scroll.h
./lib/hw.h
"

for f in $FILES; do
  indent $f -brf -linux -l10000
	rm $f~
done

