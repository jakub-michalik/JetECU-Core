#ifdef STM32F4
#include "hal/hal_system.h"
#include "hal/hal_adc.h"
#include "hal/hal_gpio.h"
#include "hal/hal_pwm.h"
#include "hal/hal_timer.h"
#include "hal/hal_nvram.h"
#include "hal/hal_watchdog.h"
#include "hal/hal_uart.h"
#include "hal/hal_can.h"
#include "core/ecu_core.h"
#include "core/ecu_config.h"

#define CONTROL_LOOP_HZ    1000
#define CONTROL_DT         (1.0f / CONTROL_LOOP_HZ)
#define WATCHDOG_TIMEOUT_MS 500

static ecu_t ecu;

int main(void)
{
    hal_system_init();
    hal_adc_init();
    hal_gpio_init();
    hal_pwm_init();
    hal_timer_init();
    hal_nvram_init();
    hal_watchdog_init(WATCHDOG_TIMEOUT_MS);

    hal_uart_config_t uart_cfg = {
        .baudrate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 'N',
    };
    hal_uart_init(HAL_UART_0, &uart_cfg);
    hal_can_init(500000);

    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    uint32_t last_tick = hal_timer_millis();

    while (1) {
        uint32_t now = hal_timer_millis();
        if (now - last_tick < (1000 / CONTROL_LOOP_HZ)) continue;
        last_tick = now;

        ecu_inputs_t inputs = {
            .rpm = hal_adc_read_voltage(HAL_ADC_CH_RPM) * 40000.0f,
            .egt = hal_adc_read_voltage(HAL_ADC_CH_EGT) * 400.0f,
            .throttle = 0.0f,
        };

        ecu_outputs_t out = ecu_step(&ecu, &inputs, CONTROL_DT);

        hal_gpio_write(HAL_GPIO_IGNITER, out.igniter_on);
        hal_gpio_write(HAL_GPIO_STARTER, out.starter_on);
        hal_pwm_set_duty(HAL_PWM_FUEL_VALVE, out.fuel_pct);

        hal_watchdog_feed();
    }
}
#endif /* STM32F4 */
