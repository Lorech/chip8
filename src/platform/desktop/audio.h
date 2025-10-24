#pragma once

#include "raylib.h"

#define SAMPLE_RATE 44100
#define FREQUENCY   220.0f
#define AMPLITUDE   0.25f
#define BUFFER_SIZE 512

// A drone sound that loops a single frequency indefinitely.
// Declaring it as an infinite audio stream allows us to not package a binary
// audio file into the program, instead just declaring it programmatically.
typedef struct {
    AudioStream stream;    // The audio stream to play back using Raylib
    bool        active;    // If the audio stream should currently be playing
    double      phase;     // The phase of the audio stream (how far along it is)
    double      increment; // The rate at which the audio stream advances
} Tone;

// Pointer to a tone that is declared outside the scope of this header.
// We need to keep a reference to it to correctly loop it for the purposes
// of this header, but the main platform implementation also needs access
// to it, so the load is shared by keeping this a pointer.
extern Tone *p_tone;

/**
 * Initialize the tone that is used as audio playback.
 *
 * @param tone - The tone to initialize
 */
void init_tone(Tone *tone);

/**
 * A callback that gets set on the audio stream (SetAudioStreamCallback).
 *
 * This callback is what ensures that the audio stream loops indefinitely
 * during the entire lifecycle of the program, as well as takes care of
 * toggling it's volume on/off based on the `active` property on the Tone.
 *
 * @param data - The audio stream buffer
 * @param frames - The number of frames that make up the audio stream
 */
static void on_audio_stream_update(void *data, unsigned int frames);
