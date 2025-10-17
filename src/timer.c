#include "timer.h"

void timer_init(timer_t *timer) {
    timer->delay = 0;
    timer->sound = 0;
}

void timer_tick(timer_t *timer) {
    if (timer->delay > 0) timer->delay--;
    if (timer->sound > 0) timer->sound--;
}
