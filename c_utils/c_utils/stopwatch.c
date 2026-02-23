#include "stopwatch.h"
#include "time_utils.h"

void stopwatch_start(stopwatch_t *sw) {
    if (!sw->running) {
        sw->start_time = time_now_ms();
        sw->running = true;
    }
}

void stopwatch_stop(stopwatch_t *sw) {
    if (sw->running) {
        sw->elapsed += (time_now_ms() - sw->start_time);
        sw->running = false;
    }
}

void stopwatch_reset(stopwatch_t *sw) {
    sw->start_time = 0;
    sw->elapsed = 0;
    sw->running = false;
}

uint64_t stopwatch_elapsed_ms(const stopwatch_t *sw) {
    if (sw->running) {
        return sw->elapsed + (time_now_ms() - sw->start_time);
    }
    return sw->elapsed;
}
