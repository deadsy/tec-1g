//-----------------------------------------------------------------------------
/*

Text Scroller

Scrolls text horizontally (either direction) on a generic puts() display.

*/
//-----------------------------------------------------------------------------

#ifndef SCROLL_H
#define SCROLL_H

//-----------------------------------------------------------------------------

struct scroll {
	const char *msg;	// message
	int len;		// message length
	int delta;		// +1/-1 scroll direction
	int width;		// width of scroll window
	void (*puts)(const char *s);	// put string function
	int n;			// current scroll state
};

//-----------------------------------------------------------------------------

// note: sizeof(buf) must be >= width
void scroll_output(struct scroll *s, char *buf);
void scroll_update(struct scroll *s, char *buf);

//-----------------------------------------------------------------------------

#endif				// SCROLL_H

//-----------------------------------------------------------------------------
