# TEC-1G Code

C-code applications for the Z80 based [TEC-1G](https://github.com/MarkJelic/TEC-1G) computer.

# Contents

 * lib - drivers and utilities for TEC-1G peripherals
 * examples - code examples

# How To Build
 * Type "make" at the top-level.
 * Linux, ok
 * MacOS, ok
 * Windows, not tested

# How To Use
 * The example applications compile to a out.ihx file (intel hex file)
 * The applications have a start address of 0x4000 (mon3 application start address)
 * Load the hex file, go to 0x4000, go ....

# Dependencies
 * [sdcc](https://sdcc.sourceforge.net/) the small device c-compiler (tested with 4.2.0)
 * GNU Make

