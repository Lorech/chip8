#include "audio.h"

#include <math.h>

#include "raylib.h"

Tone *p_tone;

void init_tone(Tone *tone) {
    p_tone          = tone;
    tone->stream    = LoadAudioStream(SAMPLE_RATE, 32, 1);
    tone->phase     = 0.0;
    tone->increment = (2.0 * PI * FREQUENCY) / SAMPLE_RATE;
    tone->active    = false;
    SetAudioStreamCallback(tone->stream, on_audio_stream_update);
}

static void on_audio_stream_update(void *data, unsigned int frames) {
    float *buffer = (float *)data;
    for (unsigned int i = 0; i < frames; i++) {
        float amp = p_tone->active ? AMPLITUDE : 0.0f;
        buffer[i] = sin(p_tone->phase) * amp;
        p_tone->phase += p_tone->increment;
        if (p_tone->phase > 2.0 * PI) p_tone->phase -= 2.0 * PI;
    }
}
