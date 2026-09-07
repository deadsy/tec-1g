#!/usr/bin/python3

import math

# experiments: Hz vs increment

# K = 718.0 / 2222.0
# K = 444.0 / 1362.0
K = 0.3245

def freq_to_increment(f):
    """convert the frequency to a table increment"""
    return float(f) / K

def midi2freq(x):
    """convert a midi note to a frequency"""
    return 440.0 * math.pow(2.0, float(x - 69) / 12.0)


NOTES_IN_OCTAVE = 12


def midi_sharp_name(n):
    sharpNotes = ("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")
    return sharpNotes[n % NOTES_IN_OCTAVE]


def midi_flat_name(n):
    flatNotes = ("C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B")
    return flatNotes[n % NOTES_IN_OCTAVE]


def midi_octave(n):
    """returns the MIDI octave of the MIDI note"""
    return int(n / NOTES_IN_OCTAVE) - 1


def midi_name(n):
    return f"{midi_sharp_name(n)}{midi_octave(n)}"


def main():
    midi_min = 21
    midi_notes = 88
    print(f"#define MIDI_NOTES {midi_notes}")
    print(f"#define MIDI_MIN {midi_min}")
    print("#define MIDI_MAX (MIDI_MIN + MIDI_NOTES - 1)")
    print("static const uint16_t note_to_increment[MIDI_NOTES] = {")
    for n in range(midi_min, midi_min + midi_notes):
        f = midi2freq(n)
        t = round(freq_to_increment(f))
        print(f"0x{t:04x}, // {n} {midi_name(n)}, {f:.1f} Hz")
    print("};")


main()
