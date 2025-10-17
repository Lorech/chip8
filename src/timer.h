#pragma once

#include <stdint.h>

typedef struct {
    uint8_t delay;
    uint8_t sound;
} timer_t;

/**
 * Initializes both timers to a starting value of 0.
 *
 * @param timer - The timer orchestrator that should be initialized
 */
void timer_init(timer_t *timer);

/**
 * Ticks (decrements) the timers if their values are above 0.
 *
 * Since the timer clock speed is independent of the CPU, this must be a public
 * API, which must be called at an appropriate rate by the timer owner.
 *
 * The CHIP-8 expects the timers to decrement at 60Hz.
 *
 * @param timer - The timer orchestrator that should tick
 */
void timer_tick(timer_t *timer);
