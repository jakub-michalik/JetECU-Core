#ifndef HAL_SYSTEM_H
#define HAL_SYSTEM_H

#include <stdint.h>

/* Board-level initialization (clock, pins, peripherals) */
int hal_system_init(void);

/* Deinitialize — safe shutdown of all peripherals */
void hal_system_deinit(void);

/* System reset */
void hal_system_reset(void);

/* Get unique board ID (if available) */
uint32_t hal_system_get_id(void);

/* Get system clock frequency in Hz */
uint32_t hal_system_get_clock_hz(void);

#endif /* HAL_SYSTEM_H */
