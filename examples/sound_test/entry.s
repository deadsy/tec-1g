; entry.s

.module entry
.globl  _main

;; Establish area ordering — these appear at the start,
;; contiguous from --code-loc, in this exact order.
.area _HEADER (ABS)
.area _HOME
.area _CODE
.area _INITIALIZER
.area _GSINIT
.area _GSFINAL

.area _DATA
.area _INITIALIZED
.area _BSEG
.area _BSS
.area _HEAP

.area _CODE

_start:
    jp _main  ; Jump directly to the C main function
