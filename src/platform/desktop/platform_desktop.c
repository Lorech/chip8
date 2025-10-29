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
    uint16_t state = 0;
    if (IsKeyDown(KEY_ONE)) state |= (0x1 << 0x1);
    if (IsKeyDown(KEY_TWO)) state |= (0x1 << 0x2);
    if (IsKeyDown(KEY_THREE)) state |= (0x1 << 0x3);
    if (IsKeyDown(KEY_FOUR)) state |= (0x1 << 0xC);
    if (IsKeyDown(KEY_Q)) state |= (0x1 << 0x4);
    if (IsKeyDown(KEY_W)) state |= (0x1 << 0x5);
    if (IsKeyDown(KEY_E)) state |= (0x1 << 0x6);
    if (IsKeyDown(KEY_R)) state |= (0x1 << 0xD);
    if (IsKeyDown(KEY_A)) state |= (0x1 << 0x7);
    if (IsKeyDown(KEY_S)) state |= (0x1 << 0x8);
    if (IsKeyDown(KEY_D)) state |= (0x1 << 0x9);
    if (IsKeyDown(KEY_F)) state |= (0x1 << 0xE);
    if (IsKeyDown(KEY_Z)) state |= (0x1 << 0xA);
    if (IsKeyDown(KEY_X)) state |= (0x1 << 0x0);
    if (IsKeyDown(KEY_C)) state |= (0x1 << 0xB);
    if (IsKeyDown(KEY_V)) state |= (0x1 << 0xF);
    return state;
}
