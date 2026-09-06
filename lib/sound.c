//-----------------------------------------------------------------------------
/*

Sound Generation

*/
//-----------------------------------------------------------------------------

#include <stdbool.h>

#include "sound.h"
#include "hw.h"

//-----------------------------------------------------------------------------

// 88-key piano
#define MIDI_NOTES 88
#define MIDI_MIN 21
#define MIDI_MAX (MIDI_MIN + MIDI_NOTES - 1)
static const int16_t note_to_increment[MIDI_NOTES] = {
	0x008b,			// 21 A0, 27.5 Hz
	0x0093,			// 22 A#0, 29.1 Hz
	0x009c,			// 23 B0, 30.9 Hz
	0x00a5,			// 24 C1, 32.7 Hz
	0x00af,			// 25 C#1, 34.6 Hz
	0x00b9,			// 26 D1, 36.7 Hz
	0x00c4,			// 27 D#1, 38.9 Hz
	0x00d0,			// 28 E1, 41.2 Hz
	0x00dc,			// 29 F1, 43.7 Hz
	0x00ea,			// 30 F#1, 46.2 Hz
	0x00f7,			// 31 G1, 49.0 Hz
	0x0106,			// 32 G#1, 51.9 Hz
	0x0116,			// 33 A1, 55.0 Hz
	0x0126,			// 34 A#1, 58.3 Hz
	0x0138,			// 35 B1, 61.7 Hz
	0x014a,			// 36 C2, 65.4 Hz
	0x015e,			// 37 C#2, 69.3 Hz
	0x0173,			// 38 D2, 73.4 Hz
	0x0189,			// 39 D#2, 77.8 Hz
	0x01a0,			// 40 E2, 82.4 Hz
	0x01b9,			// 41 F2, 87.3 Hz
	0x01d3,			// 42 F#2, 92.5 Hz
	0x01ef,			// 43 G2, 98.0 Hz
	0x020c,			// 44 G#2, 103.8 Hz
	0x022c,			// 45 A2, 110.0 Hz
	0x024d,			// 46 A#2, 116.5 Hz
	0x0270,			// 47 B2, 123.5 Hz
	0x0295,			// 48 C3, 130.8 Hz
	0x02bc,			// 49 C#3, 138.6 Hz
	0x02e6,			// 50 D3, 146.8 Hz
	0x0312,			// 51 D#3, 155.6 Hz
	0x0340,			// 52 E3, 164.8 Hz
	0x0372,			// 53 F3, 174.6 Hz
	0x03a6,			// 54 F#3, 185.0 Hz
	0x03de,			// 55 G3, 196.0 Hz
	0x0419,			// 56 G#3, 207.7 Hz
	0x0457,			// 57 A3, 220.0 Hz
	0x0499,			// 58 A#3, 233.1 Hz
	0x04df,			// 59 B3, 246.9 Hz
	0x0529,			// 60 C4, 261.6 Hz
	0x0578,			// 61 C#4, 277.2 Hz
	0x05cb,			// 62 D4, 293.7 Hz
	0x0623,			// 63 D#4, 311.1 Hz
	0x0681,			// 64 E4, 329.6 Hz
	0x06e4,			// 65 F4, 349.2 Hz
	0x074d,			// 66 F#4, 370.0 Hz
	0x07bc,			// 67 G4, 392.0 Hz
	0x0831,			// 68 G#4, 415.3 Hz
	0x08ae,			// 69 A4, 440.0 Hz
	0x0932,			// 70 A#4, 466.2 Hz
	0x09be,			// 71 B4, 493.9 Hz
	0x0a53,			// 72 C5, 523.3 Hz
	0x0af0,			// 73 C#5, 554.4 Hz
	0x0b96,			// 74 D5, 587.3 Hz
	0x0c47,			// 75 D#5, 622.3 Hz
	0x0d02,			// 76 E5, 659.3 Hz
	0x0dc8,			// 77 F5, 698.5 Hz
	0x0e99,			// 78 F#5, 740.0 Hz
	0x0f78,			// 79 G5, 784.0 Hz
	0x1063,			// 80 G#5, 830.6 Hz
	0x115c,			// 81 A5, 880.0 Hz
	0x1265,			// 82 A#5, 932.3 Hz
	0x137d,			// 83 B5, 987.8 Hz
	0x14a5,			// 84 C6, 1046.5 Hz
	0x15e0,			// 85 C#6, 1108.7 Hz
	0x172d,			// 86 D6, 1174.7 Hz
	0x188d,			// 87 D#6, 1244.5 Hz
	0x1a03,			// 88 E6, 1318.5 Hz
	0x1b8f,			// 89 F6, 1396.9 Hz
	0x1d33,			// 90 F#6, 1480.0 Hz
	0x1eef,			// 91 G6, 1568.0 Hz
	0x20c6,			// 92 G#6, 1661.2 Hz
	0x22b9,			// 93 A6, 1760.0 Hz
	0x24c9,			// 94 A#6, 1864.7 Hz
	0x26f9,			// 95 B6, 1975.5 Hz
	0x294b,			// 96 C7, 2093.0 Hz
	0x2bbf,			// 97 C#7, 2217.5 Hz
	0x2e59,			// 98 D7, 2349.3 Hz
	0x311b,			// 99 D#7, 2489.0 Hz
	0x3406,			// 100 E7, 2637.0 Hz
	0x371e,			// 101 F7, 2793.8 Hz
	0x3a65,			// 102 F#7, 2960.0 Hz
	0x3dde,			// 103 G7, 3136.0 Hz
	0x418c,			// 104 G#7, 3322.4 Hz
	0x4572,			// 105 A7, 3520.0 Hz
	0x4993,			// 106 A#7, 3729.3 Hz
	0x4df3,			// 107 B7, 3951.1 Hz
	0x5295,			// 108 C8, 4186.0 Hz
};

// map a midi note to a table increment
static int16_t midi_to_increment(uint8_t n) {
	if ((n < MIDI_MIN) || (n > MIDI_MAX)) {
		return -1;
	}
	return note_to_increment[n - MIDI_MIN];
}

//-----------------------------------------------------------------------------
// Note: we're not trying to preserve the value of other bits on digitPort.
// The port is write-only, so we can't do a RMW.

static inline void speaker_hi(void) {
	digitPort = (uint8_t) speakerMask;
}

static inline void speaker_lo(void) {
	digitPort = 0;
}

//-----------------------------------------------------------------------------

void play_square(uint16_t inc, uint16_t duty, uint16_t cycles) {
	uint16_t ofs = 0;
	bool hi = true;
	speaker_hi();
	while (cycles > 0) {
		if (ofs < duty) {
			if (!hi) {
				hi = true;
				speaker_hi();
			}
		} else {
			if (hi) {
				speaker_lo();
				hi = false;
			}
		}
		ofs += inc;
		cycles--;
	}
	speaker_lo();
}

//-----------------------------------------------------------------------------

void play_note(uint8_t note, uint16_t duration) {
	int16_t inc = midi_to_increment(note);
	if ((inc < 0) || (duration == 0)) {
		return;
	}
	play_square(inc, 32768, duration);
}

void play_tune(uint8_t *tune) {
	(void)tune;
}

//-----------------------------------------------------------------------------
