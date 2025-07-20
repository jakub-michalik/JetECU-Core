#ifdef ESP_PLATFORM
#include <stdio.h>
#include <stdbool.h>
#include "hal/hal_system.h"
#include "hal/hal_adc.h"
#include "hal/hal_gpio.h"
#include "hal/hal_pwm.h"
#include "hal/hal_timer.h"
#include "hal/hal_nvram.h"
#include "hal/hal_watchdog.h"
#include "core/ecu_core.h"
#include "core/ecu_config.h"

#define CONTROL_LOOP_HZ   1000
#define CONTROL_DT        (1.0f / CONTROL_LOOP_HZ)
#define WATCHDOG_TIMEOUT_MS 500
#define PRINT_INTERVAL_MS  1000

static ecu_t ecu;

void app_main(void)
{
    printf("JetECU v0.1 starting...\n");

    hal_system_init();
    hal_adc_init();
    hal_gpio_init();
    hal_pwm_init();
    hal_timer_init();
    hal_nvram_init();
    hal_watchdog_init(WATCHDOG_TIMEOUT_MS);

    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    printf("ECU initialized (RPM idle=%.0f, EGT max=%.0f)\n",
           cfg.rpm_idle, cfg.egt_max);

    uint32_t last_control = hal_timer_millis();
    uint32_t last_print = 0;

    while (1) {
        uint32_t now = hal_timer_millis();
        uint32_t elapsed = now - last_control;
        if (elapsed < (1000 / CONTROL_LOOP_HZ)) {
            continue;
        }
        last_control = now;

        ecu_inputs_t inputs = {
            .rpm = hal_adc_read_voltage(HAL_ADC_CH_RPM) * 40000.0f,
            .egt = hal_adc_read_voltage(HAL_ADC_CH_EGT) * 400.0f,
            .throttle = 0.0f,
        };

        ecu_outputs_t out = ecu_step(&ecu, &inputs, CONTROL_DT);

        hal_gpio_write(HAL_GPIO_IGNITER, out.igniter_on);
        hal_gpio_write(HAL_GPIO_STARTER, out.starter_on);
        hal_pwm_set_duty(HAL_PWM_FUEL_VALVE, out.fuel_pct);
        hal_gpio_write(HAL_GPIO_LED_STATUS,
            out.state == ECU_STATE_RUN || ((now / 500) % 2 == 0));

        /* Debug print */
        if (now - last_print >= PRINT_INTERVAL_MS) {
            printf("[%s] RPM=%.0f EGT=%.0f Fuel=%.1f%%\n",
                   ecu_sm_state_name(out.state),
                   inputs.rpm, inputs.egt, out.fuel_pct);
            last_print = now;
        }

        hal_watchdog_feed();
    }
}
#endif /* ESP_PLATFORM */
