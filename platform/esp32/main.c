#ifdef ESP_PLATFORM
#include <stdio.h>
#include "hal/hal_system.h"
#include "hal/hal_adc.h"
#include "hal/hal_gpio.h"
#include "hal/hal_pwm.h"
#include "hal/hal_timer.h"
#include "hal/hal_nvram.h"
#include "hal/hal_watchdog.h"
#include "hal/hal_uart.h"
#include "core/ecu_core.h"
#include "core/ecu_config.h"

#define CONTROL_LOOP_HZ  1000
#define CONTROL_DT        (1.0f / CONTROL_LOOP_HZ)
#define WATCHDOG_TIMEOUT_MS 500

static ecu_t ecu;

void app_main(void)
{
    printf("JetECU starting...\n");

    /* Initialize HAL */
    hal_system_init();
    hal_adc_init();
    hal_gpio_init();
    hal_pwm_init();
    hal_timer_init();
    hal_nvram_init();
    hal_watchdog_init(WATCHDOG_TIMEOUT_MS);

    /* Initialize ECU */
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    printf("ECU initialized, entering control loop\n");

    uint32_t last_tick = hal_timer_millis();

    while (1) {
        uint32_t now = hal_timer_millis();
        if (now - last_tick < (1000 / CONTROL_LOOP_HZ)) {
            continue;  /* busy-wait for next tick */
        }
        last_tick = now;

        /* Read sensors */
        ecu_inputs_t inputs;
        inputs.rpm = hal_adc_read_voltage(HAL_ADC_CH_RPM) * 40000.0f;  /* scale factor */
        inputs.egt = hal_adc_read_voltage(HAL_ADC_CH_EGT) * 400.0f;
        inputs.throttle = 0.0f;  /* TODO: read from telemetry */

        /* Run ECU step */
        ecu_outputs_t out = ecu_step(&ecu, &inputs, CONTROL_DT);

        /* Apply outputs */
        hal_gpio_write(HAL_GPIO_IGNITER, out.igniter_on);
        hal_gpio_write(HAL_GPIO_STARTER, out.starter_on);
        hal_pwm_set_duty(HAL_PWM_FUEL_VALVE, out.fuel_pct);

        /* Status LED */
        hal_gpio_write(HAL_GPIO_LED_STATUS,
            (out.state == ECU_STATE_RUN) || (now / 500 % 2 == 0));

        hal_watchdog_feed();
    }
}
#endif /* ESP_PLATFORM */
