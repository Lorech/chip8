#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "audio.h"
#include "platform.h"
#include "raylib.h"

static uint8_t display_width;
static uint8_t display_height;
static Tone    tone;

void platform_init(uint8_t width, uint8_t height, uint8_t fps) {
    display_width  = width;
    display_height = height;

    SetTraceLogLevel(LOG_WARNING);

    SetTargetFPS(fps);
    InitWindow(width, height, "CHIP-8");

    InitAudioDevice();
    init_tone(&tone);
    PlayAudioStream(tone.stream);
}

void platform_close() {
    CloseAudioDevice();
    CloseWindow();
}

void platform_sleep(uint64_t microseconds) {
#ifdef _WIN32
    Sleep(DWORD(microseconds / 1000));
#else
    usleep(microseconds);
#endif
}

uint64_t platform_get_time(void) {
#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000) / freq.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)(ts.tv_nsec / 1000);
#endif
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
