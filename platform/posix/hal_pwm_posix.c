#include "hal/hal_pwm.h"

static float pwm_duty[HAL_PWM_COUNT] = {0};

int hal_pwm_init(void) { return 0; }

void hal_pwm_set_duty(hal_pwm_channel_t ch, float duty_pct)
{
    if (ch < HAL_PWM_COUNT) pwm_duty[ch] = duty_pct;
}

void hal_pwm_set_servo_us(hal_pwm_channel_t ch, uint16_t us)
{
    /* Convert us to duty % (assume 50Hz, 20ms period) */
    if (ch < HAL_PWM_COUNT) {
        pwm_duty[ch] = (float)us / 20000.0f * 100.0f;
    }
}

float hal_pwm_posix_get_duty(hal_pwm_channel_t ch)
{
    return (ch < HAL_PWM_COUNT) ? pwm_duty[ch] : 0.0f;
}
