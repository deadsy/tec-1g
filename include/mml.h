//-----------------------------------------------------------------------------
/*

Music Macro Language

Represent a tune as a C string with plain text.
Interpret it to a midi note and duration sequence.

https://en.wikipedia.org/wiki/Music_Macro_Language

*/
//-----------------------------------------------------------------------------

#ifndef MML_H
#define MML_H

//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------

struct mml_note {
	uint8_t note;
	uint16_t duration;
};

struct mml_state {
	const char *song;	// cursor into the tune string
	uint8_t octave;		// current octave (0..8), middle C == octave 4
	uint8_t length;		// default note length (1,2,4,8,16,32)
	uint16_t tempo;		// tempo in beats (quarter notes) per minute
};

//-----------------------------------------------------------------------------

// initialise the mml parser for the given song. returns -1 on bad args.
int8_t mml_init(struct mml_state *mml, const char *song);

// get the next mml note. returns 1 on a note, 0 at end of song, -1 on error.
int8_t mml_next(struct mml_state *mml, struct mml_note *note);

//-----------------------------------------------------------------------------

#endif				// MML_H

//-----------------------------------------------------------------------------
