//-----------------------------------------------------------------------------
/*

Text Scroller

Scrolls text horizontally (either direction) on a generic puts() display.

*/
//-----------------------------------------------------------------------------

#include "scroll.h"

//-----------------------------------------------------------------------------

// return a mod b
static int scroll_mod(int a, int b) {
	if (b < 0) {
		b = -b;
	}
	while (a < 0) {
		a += b;
	}
	while (a >= b) {
		a -= b;
	}
	return a;
}

void scroll_output(struct scroll *s, char *buf) {
	for (int i = 0; i < s->width; i++) {
		buf[i] = s->msg[scroll_mod(s->n + i, s->len)];
	}
	s->puts(buf);
}

void scroll_update(struct scroll *s, char *buf) {
	s->n = scroll_mod(s->n + s->delta, s->len);
	scroll_output(s, buf);
}

//-----------------------------------------------------------------------------
