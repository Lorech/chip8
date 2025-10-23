#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "platform.h"
#include "raylib.h"

#define SAMPLE_RATE 44100
#define FREQUENCY   220.0f
#define AMPLITUDE   0.25f
#define BUFFER_SIZE 512

typedef struct {
    AudioStream stream;
    bool        active;
    double      phase;
    double      increment;
} Tone;

static uint8_t display_width;
static uint8_t display_height;
static Tone    tone;

static void OnAudioStreamUpdate(void *bufferData, unsigned int frames) {
    float *buffer = (float *)bufferData;
    for (unsigned int i = 0; i < frames; i++) {
        float amp = tone.active ? AMPLITUDE : 0.0f;
        buffer[i] = sin(tone.phase) * amp;
        tone.phase += tone.increment;
        if (tone.phase > 2.0 * PI) tone.phase -= 2.0 * PI;
    }
}

void platform_init(uint8_t width, uint8_t height, uint8_t fps) {
    display_width  = width;
    display_height = height;

    SetTraceLogLevel(LOG_WARNING);

    SetTargetFPS(fps);
    InitWindow(width, height, "CHIP-8");

    InitAudioDevice();
    tone.stream    = LoadAudioStream(SAMPLE_RATE, 32, 1);
    tone.phase     = 0.0;
    tone.increment = (2.0 * PI * FREQUENCY) / SAMPLE_RATE;
    tone.active    = false;
    SetAudioStreamCallback(tone.stream, OnAudioStreamUpdate);
    PlayAudioStream(tone.stream);
}

void platform_close() {
    CloseAudioDevice();
    CloseWindow();
}

void platform_sleep(double seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    usleep(seconds * 1000000);
#endif
}

uint64_t platform_get_time(void) {
    return time(NULL);
}

void platform_seed_rng(uint64_t seed) {
    srand(seed);
}

uint8_t platform_rng(void) {
    return (uint8_t)rand();
}

bool platform_load_rom(uint8_t *rom, size_t max_size, int argc, char **argv) {
    if (argc < 2) return false;

    char *path = argv[1];
    if (!path) return false;

    FILE *infile = fopen(path, "rb");
    if (!infile) return false;

    size_t result = fread(rom, sizeof(uint8_t), max_size, infile);

    if (ferror(infile)) {
        fclose(infile);
        return false;
    }

    fclose(infile);
    return true;
}

void platform_draw_display(bool *buffer) {
    BeginDrawing();
    for (uint8_t x = 0; x < display_width; ++x) {
        for (uint8_t y = 0; y < display_height; ++y) {
            if (buffer[y * display_width + x]) {
                DrawPixel(x, y, RAYWHITE);
            }
        }
    }
    EndDrawing();
}

void platform_play_audio(void) {
    tone.active = true;
}

void platform_stop_audio(void) {
    tone.active = false;
}

uint16_t platform_get_keypad(void) {
    // TODO: Add implementation.
    return 0;
}
