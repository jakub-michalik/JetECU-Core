#ifdef ESP_PLATFORM
#include "hal/hal_pwm.h"
#include "driver/ledc.h"

#define PWM_FREQ_HZ     50     /* 50Hz for servo */
#define PWM_RESOLUTION  LEDC_TIMER_13_BIT

static const ledc_channel_t ch_map[] = {
    [HAL_PWM_FUEL_VALVE] = LEDC_CHANNEL_0,
};

static const int gpio_map[] = {
    [HAL_PWM_FUEL_VALVE] = 14,
};

int hal_pwm_init(void)
{
    ledc_timer_config_t timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_cfg);

    for (int i = 0; i < HAL_PWM_COUNT; i++) {
        ledc_channel_config_t ch_cfg = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = ch_map[i],
            .timer_sel = LEDC_TIMER_0,
            .intr_type = LEDC_INTR_DISABLE,
            .gpio_num = gpio_map[i],
            .duty = 0,
            .hpoint = 0,
        };
        ledc_channel_config(&ch_cfg);
    }
    return 0;
}

void hal_pwm_set_duty(hal_pwm_channel_t ch, float duty_pct)
{
    if (ch >= HAL_PWM_COUNT) return;
    uint32_t max_duty = (1 << PWM_RESOLUTION) - 1;
    uint32_t duty = (uint32_t)(duty_pct / 100.0f * max_duty);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, ch_map[ch], duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, ch_map[ch]);
}

void hal_pwm_set_servo_us(hal_pwm_channel_t ch, uint16_t us)
{
    /* Convert microseconds to duty for 50Hz (20ms period) */
    float duty_pct = (float)us / 20000.0f * 100.0f;
    hal_pwm_set_duty(ch, duty_pct);
}
#endif /* ESP_PLATFORM */
