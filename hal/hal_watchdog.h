#ifndef HAL_WATCHDOG_H
#define HAL_WATCHDOG_H

#include <stdint.h>

/* Initialize watchdog with timeout in milliseconds */
int hal_watchdog_init(uint32_t timeout_ms);

/* Feed / kick the watchdog */
void hal_watchdog_feed(void);

#endif /* HAL_WATCHDOG_H */
