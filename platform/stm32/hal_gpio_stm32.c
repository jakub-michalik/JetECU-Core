#ifdef STM32F4
#include "hal/hal_gpio.h"
#include "stm32f4xx_hal.h"
#include "platform/stm32/stm32_hal_conf.h"

static const struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} pin_map[] = {
    [HAL_GPIO_IGNITER]     = { STM32_PORT_IGNITER,  STM32_PIN_IGNITER  },
    [HAL_GPIO_STARTER]     = { STM32_PORT_STARTER,  STM32_PIN_STARTER  },
    [HAL_GPIO_FUEL_ENABLE] = { STM32_PORT_FUEL_EN,  STM32_PIN_FUEL_EN  },
    [HAL_GPIO_LED_STATUS]  = { STM32_PORT_LED,      STM32_PIN_LED      },
};

int hal_gpio_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef init = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };

    for (int i = 0; i < HAL_GPIO_COUNT; i++) {
        init.Pin = pin_map[i].pin;
        HAL_GPIO_Init(pin_map[i].port, &init);
        HAL_GPIO_WritePin(pin_map[i].port, pin_map[i].pin, GPIO_PIN_RESET);
    }
    return 0;
}

void hal_gpio_set_dir(hal_gpio_pin_t pin, hal_gpio_dir_t dir)
{
    (void)pin;
    (void)dir;
    /* Reconfigure would need full GPIO_Init — not commonly done at runtime */
}

void hal_gpio_write(hal_gpio_pin_t pin, bool state)
{
    if (pin >= HAL_GPIO_COUNT) return;
    HAL_GPIO_WritePin(pin_map[pin].port, pin_map[pin].pin,
                      state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool hal_gpio_read(hal_gpio_pin_t pin)
{
    if (pin >= HAL_GPIO_COUNT) return false;
    return HAL_GPIO_ReadPin(pin_map[pin].port, pin_map[pin].pin) == GPIO_PIN_SET;
}
#endif /* STM32F4 */
