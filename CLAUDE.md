# CLAUDE.md

This file guides Claude Code when working in this repository.

## Project Overview

C-code applications and drivers for the **TEC-1G**, a Z80-based single-board
computer (https://github.com/MarkJelic/TEC-1G). Code is cross-compiled with
**SDCC** (Small Device C Compiler) targeting the Z80, and loaded onto the
machine as Intel HEX files.

- `lib/` — drivers and utilities for TEC-1G peripherals, built into `tec1.lib`
- `examples/` — standalone example applications, each compiling to `out.ihx`
- `include/` — public headers for the library

## Building

- `make` at the top level builds `lib/` then `examples/`.
- `make clean` removes all generated artifacts.
- Requires `sdcc` (tested with 4.2.0) and GNU Make. Works on Linux/macOS;
  Windows untested.
- Build artifacts (`*.ihx *.rel *.asm *.lst *.sym *.lk *.map *.noi *.lib`) are
  gitignored — do not commit them.

## Running on Hardware

- Each example compiles to `out.ihx` (Intel HEX).
- Applications link at start address **0x4000** (the mon3 application start
  address). Load the hex file, go to 0x4000, run.
- Default memory layout (see `examples/common.mk`): `CODE=0x4000`,
  `DATA=0x7000`, `STACK=0x7ffe`. Override via make variables.

## Architecture & Conventions

- **Compiler flags**: `-mz80 --Werror --std-c99 -DHW_TEC_1G`. Treat warnings as
  errors. `GIT_HASH` is passed as a define in the library build.
- **Hardware access**: peripheral ports are declared as `__sfr __at 0xNN` in
  `include/hw.h`. All I/O port addresses and bit masks live there — reference it
  rather than hardcoding port numbers.
- **Entry point**: examples use a custom `entry.s` (`--no-std-crt0`) that jumps
  directly to `_main`. No standard C runtime startup.
- **Library structure**: each driver is a `.c`/`.h` pair (e.g. `lcd`, `glcd`,
  `keypad`, `rtc`, `sdcard`, `fat`, `term`, `sound`/`mml`, `menu`, `scroll`,
  `display`, `memshow`, `array88`, `font`, `delay`). Add new drivers to the
  `OBJS` list in `lib/Makefile`.
- **New example**: create a directory under `examples/`, add `entry.s`, `main.c`
  and a `Makefile` containing `OBJS = entry.rel main.rel` + `include ../common.mk`,
  then register the directory in `examples/Makefile`.

## Formatting

- Run `./format.sh` to format all sources with GNU `indent`
  (`-brf -linux -l10000`). Run it before committing style changes.
