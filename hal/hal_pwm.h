#ifndef HAL_PWM_H
#define HAL_PWM_H

#include <stdint.h>

typedef enum {
    HAL_PWM_FUEL_VALVE = 0,
    HAL_PWM_COUNT,
} hal_pwm_channel_t;

int hal_pwm_init(void);

/* Set duty cycle 0-100% */
void hal_pwm_set_duty(hal_pwm_channel_t ch, float duty_pct);

/* Set servo position in microseconds (1000-2000 typical) */
void hal_pwm_set_servo_us(hal_pwm_channel_t ch, uint16_t us);

#endif /* HAL_PWM_H */
