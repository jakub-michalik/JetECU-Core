#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>

/* Initialize timer subsystem */
int hal_timer_init(void);

/* Get current time in milliseconds since boot */
uint32_t hal_timer_millis(void);

/* Get current time in microseconds */
uint32_t hal_timer_micros(void);

/* Delay (blocking) */
void hal_timer_delay_ms(uint32_t ms);

#endif /* HAL_TIMER_H */
