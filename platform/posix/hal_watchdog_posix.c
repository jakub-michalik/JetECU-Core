#include "hal/hal_watchdog.h"

/* No-op watchdog for POSIX */
int hal_watchdog_init(uint32_t timeout_ms)
{
    (void)timeout_ms;
    return 0;
}

void hal_watchdog_feed(void) {}
