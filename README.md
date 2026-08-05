# TEC-1G Code

C-code applications for the Z80 based TEC-1G computer.

# Contents

 * lib - drivers for TEC-1G peripherals
 * examples - code examples

# How To Build
 * Use Linux :-)
 * Type "make" at the top-level.

# How To Use
 * The code examples compile to an out.ihx file (intel hex file)
 * The hex files link to 0x4000 (mon3 application start address)
 * load the hex file, go to 0x4000, go ....

# Dependencies
 * sdcc - small device c-compiler https://sdcc.sourceforge.net/

