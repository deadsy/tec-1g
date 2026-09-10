//-----------------------------------------------------------------------------
/*

Music Macro Language

Represent a tune as a C string with plain text.
Interpret it to a midi note and duration sequence.

https://en.wikipedia.org/wiki/Music_Macro_Language

*/
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "mml.h"
#include "sound.h"

//-----------------------------------------------------------------------------

#define OCTAVE_DEFAULT 4
#define LENGTH_DEFAULT 4
#define TEMPO_DEFAULT 120

#define OCTAVE_MAX 8
#define MIDI_MAX 127

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

// semitone offset from C for note letters A..G
static const uint8_t note_semitone[7] = {
	9,			// A
	11,			// B
	0,			// C
	2,			// D
	4,			// E
	5,			// F
	7,			// G
};

//-----------------------------------------------------------------------------

static bool is_digit(char c) {
	return (c >= '0') && (c <= '9');
}

// parse an unsigned decimal number, advancing the cursor. return true if any
// digit was consumed, storing the value in *out.
static bool parse_number(const char **p, uint16_t *out) {
	uint16_t val = 0;
	bool found = false;
	while (is_digit(**p)) {
		val = (val * 10) + (uint16_t) (**p - '0');
		(*p)++;
		found = true;
	}
	if (found) {
		*out = val;
	}
	return found;
}

// compute the duration of a note in milliseconds for the given length code,
// applying any trailing dots that extend the duration.
static uint16_t note_duration(struct mml_state *mml, uint16_t length) {
	if (length == 0) {
		length = mml->length;
	}
	// a quarter note (length 4) lasts 60000/tempo ms.
	// a note of the given length lasts (4 / length) quarter notes.
	uint32_t ms = (uint32_t) 4 * 60000UL / ((uint32_t) mml->tempo * length);

	// dotted notes: each dot adds half of the previous value.
	uint32_t add = ms;
	while (*mml->song == '.') {
		add /= 2;
		ms += add;
		mml->song++;
	}
	if (ms > 0xffff) {
		ms = 0xffff;
	}
	return (uint16_t) ms;
}

//-----------------------------------------------------------------------------

// get the next mml note.
// return 1 on a note, 0 at end of song, -1 on error.
static int8_t mml_next(struct mml_state *mml, struct mml_note *note) {
	if ((mml == NULL) || (note == NULL) || (mml->song == NULL)) {
		return -1;
	}

	while (*mml->song != 0) {
		char c = *mml->song++;
		uint16_t num;

		// upper-case letters for convenience
		if ((c >= 'a') && (c <= 'z')) {
			c = (char)(c - 'a' + 'A');
		}

		switch (c) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			// whitespace is ignored
			break;

		case 'O':	// set octave
			if (parse_number(&mml->song, &num) && (num <= OCTAVE_MAX)) {
				mml->octave = (uint8_t) num;
			}
			break;

		case '>':	// octave up
			if (mml->octave < OCTAVE_MAX) {
				mml->octave++;
			}
			break;

		case '<':	// octave down
			if (mml->octave > 0) {
				mml->octave--;
			}
			break;

		case 'L':	// set default note length
			if (parse_number(&mml->song, &num) && (num > 0)) {
				mml->length = (uint8_t) num;
			}
			break;

		case 'T':	// set tempo
			if (parse_number(&mml->song, &num) && (num > 0)) {
				mml->tempo = num;
			}
			break;

		case 'R':	// rest
		case 'P':	// pause (alias)
			num = 0;
			parse_number(&mml->song, &num);
			note->note = NOTE_REST;
			note->duration = note_duration(mml, num);
			return 1;

		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':{
				int16_t semitone = note_semitone[c - 'A'];

				// accidentals
				if (*mml->song == '#' || *mml->song == '+') {
					semitone++;
					mml->song++;
				} else if (*mml->song == '-') {
					semitone--;
					mml->song++;
				}

				num = 0;
				parse_number(&mml->song, &num);

				int16_t midi = (int16_t) ((mml->octave + 1) * 12) + semitone;
				if (midi > MIDI_MAX) {
					midi = MIDI_MAX;
				}

				note->note = (uint8_t) midi;
				note->duration = note_duration(mml, num);
				return 1;
			}

		default:
			// unknown character, skip it
			break;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------

// initialise the mml parser for the given song. returns -1 on bad args.
static int8_t mml_init(struct mml_state *mml, const char *song) {
	if ((mml == NULL) || (song == NULL) || (*song == 0)) {
		return -1;
	}
	memset(mml, 0, sizeof(struct mml_state));
	mml->song = song;
	mml->octave = OCTAVE_DEFAULT;
	mml->length = LENGTH_DEFAULT;
	mml->tempo = TEMPO_DEFAULT;
	return 0;
}

//-----------------------------------------------------------------------------

// play_note() takes a duration as a loop-cycle count
// mml_next() returns milliseconds.
// This is the conversion factor.
// by experiment: 50000 -> 2.368s
// So: 50000/2368 ~ 21

#define MS_TO_CYCLES 21

// play an mml song
int8_t mml_play(const char *song) {
	struct mml_state s;
	if (mml_init(&s, song) != 0) {
		return -1;
	}
	struct mml_note note;
	while (mml_next(&s, &note) == 1) {
		play_note(note.note, note.duration * MS_TO_CYCLES);
	}
	return 0;
}

//-----------------------------------------------------------------------------
