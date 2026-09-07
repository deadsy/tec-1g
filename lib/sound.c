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
static const uint16_t note_to_increment[MIDI_NOTES] = {
	0x0055,			// 21 A0, 27.5 Hz
	0x005a,			// 22 A#0, 29.1 Hz
	0x005f,			// 23 B0, 30.9 Hz
	0x0065,			// 24 C1, 32.7 Hz
	0x006b,			// 25 C#1, 34.6 Hz
	0x0071,			// 26 D1, 36.7 Hz
	0x0078,			// 27 D#1, 38.9 Hz
	0x007f,			// 28 E1, 41.2 Hz
	0x0087,			// 29 F1, 43.7 Hz
	0x008f,			// 30 F#1, 46.2 Hz
	0x0097,			// 31 G1, 49.0 Hz
	0x00a0,			// 32 G#1, 51.9 Hz
	0x00a9,			// 33 A1, 55.0 Hz
	0x00b4,			// 34 A#1, 58.3 Hz
	0x00be,			// 35 B1, 61.7 Hz
	0x00ca,			// 36 C2, 65.4 Hz
	0x00d6,			// 37 C#2, 69.3 Hz
	0x00e2,			// 38 D2, 73.4 Hz
	0x00f0,			// 39 D#2, 77.8 Hz
	0x00fe,			// 40 E2, 82.4 Hz
	0x010d,			// 41 F2, 87.3 Hz
	0x011d,			// 42 F#2, 92.5 Hz
	0x012e,			// 43 G2, 98.0 Hz
	0x0140,			// 44 G#2, 103.8 Hz
	0x0153,			// 45 A2, 110.0 Hz
	0x0167,			// 46 A#2, 116.5 Hz
	0x017c,			// 47 B2, 123.5 Hz
	0x0193,			// 48 C3, 130.8 Hz
	0x01ab,			// 49 C#3, 138.6 Hz
	0x01c4,			// 50 D3, 146.8 Hz
	0x01df,			// 51 D#3, 155.6 Hz
	0x01fc,			// 52 E3, 164.8 Hz
	0x021a,			// 53 F3, 174.6 Hz
	0x023a,			// 54 F#3, 185.0 Hz
	0x025c,			// 55 G3, 196.0 Hz
	0x0280,			// 56 G#3, 207.7 Hz
	0x02a6,			// 57 A3, 220.0 Hz
	0x02ce,			// 58 A#3, 233.1 Hz
	0x02f9,			// 59 B3, 246.9 Hz
	0x0326,			// 60 C4, 261.6 Hz
	0x0356,			// 61 C#4, 277.2 Hz
	0x0389,			// 62 D4, 293.7 Hz
	0x03bf,			// 63 D#4, 311.1 Hz
	0x03f8,			// 64 E4, 329.6 Hz
	0x0434,			// 65 F4, 349.2 Hz
	0x0474,			// 66 F#4, 370.0 Hz
	0x04b8,			// 67 G4, 392.0 Hz
	0x0500,			// 68 G#4, 415.3 Hz
	0x054c,			// 69 A4, 440.0 Hz
	0x059d,			// 70 A#4, 466.2 Hz
	0x05f2,			// 71 B4, 493.9 Hz
	0x064c,			// 72 C5, 523.3 Hz
	0x06ac,			// 73 C#5, 554.4 Hz
	0x0712,			// 74 D5, 587.3 Hz
	0x077e,			// 75 D#5, 622.3 Hz
	0x07f0,			// 76 E5, 659.3 Hz
	0x0868,			// 77 F5, 698.5 Hz
	0x08e8,			// 78 F#5, 740.0 Hz
	0x0970,			// 79 G5, 784.0 Hz
	0x0a00,			// 80 G#5, 830.6 Hz
	0x0a98,			// 81 A5, 880.0 Hz
	0x0b39,			// 82 A#5, 932.3 Hz
	0x0be4,			// 83 B5, 987.8 Hz
	0x0c99,			// 84 C6, 1046.5 Hz
	0x0d59,			// 85 C#6, 1108.7 Hz
	0x0e24,			// 86 D6, 1174.7 Hz
	0x0efb,			// 87 D#6, 1244.5 Hz
	0x0fdf,			// 88 E6, 1318.5 Hz
	0x10d1,			// 89 F6, 1396.9 Hz
	0x11d1,			// 90 F#6, 1480.0 Hz
	0x12e0,			// 91 G6, 1568.0 Hz
	0x13ff,			// 92 G#6, 1661.2 Hz
	0x1530,			// 93 A6, 1760.0 Hz
	0x1672,			// 94 A#6, 1864.7 Hz
	0x17c8,			// 95 B6, 1975.5 Hz
	0x1932,			// 96 C7, 2093.0 Hz
	0x1ab1,			// 97 C#7, 2217.5 Hz
	0x1c48,			// 98 D7, 2349.3 Hz
	0x1df6,			// 99 D#7, 2489.0 Hz
	0x1fbe,			// 100 E7, 2637.0 Hz
	0x21a2,			// 101 F7, 2793.8 Hz
	0x23a2,			// 102 F#7, 2960.0 Hz
	0x25c0,			// 103 G7, 3136.0 Hz
	0x27ff,			// 104 G#7, 3322.4 Hz
	0x2a5f,			// 105 A7, 3520.0 Hz
	0x2ce4,			// 106 A#7, 3729.3 Hz
	0x2f90,			// 107 B7, 3951.1 Hz
	0x3264,			// 108 C8, 4186.0 Hz
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

static inline void speaker_out(uint8_t val) {
	digitPort = val;
}

// play a square wave
void play_square(uint16_t freq, uint16_t duty, uint16_t cycles) {
	uint16_t ofs = 0;
	speaker_out(0);
	while (cycles > 0) {
		speaker_out((ofs > duty) ? speakerMask : 0);
		ofs += freq;
		cycles--;
	}
	speaker_out(0);
}

//-----------------------------------------------------------------------------

// play a midi note
void play_note(uint8_t note, uint16_t duration) {
	if (duration == 0) {
		return;
	}
	if (note == NOTE_REST) {
		// duty of 0xffff keeps ofs <= duty always true => speaker stays low
		play_square(0, 0xffff, duration);
		return;
	}
	int16_t inc = midi_to_increment(note);
	if (inc < 0) {
		return;
	}
	play_square(inc, 32768, duration);
}

//-----------------------------------------------------------------------------
