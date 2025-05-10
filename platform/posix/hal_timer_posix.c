#include "hal/hal_timer.h"
#include <time.h>
#include <unistd.h>

static struct timespec start_time;
static int timer_initialized = 0;

int hal_timer_init(void)
{
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    timer_initialized = 1;
    return 0;
}

uint32_t hal_timer_millis(void)
{
    if (!timer_initialized) hal_timer_init();
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint32_t)((now.tv_sec - start_time.tv_sec) * 1000 +
                      (now.tv_nsec - start_time.tv_nsec) / 1000000);
}

uint32_t hal_timer_micros(void)
{
    if (!timer_initialized) hal_timer_init();
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint32_t)((now.tv_sec - start_time.tv_sec) * 1000000 +
                      (now.tv_nsec - start_time.tv_nsec) / 1000);
}

void hal_timer_delay_ms(uint32_t ms)
{
    usleep(ms * 1000);
}
