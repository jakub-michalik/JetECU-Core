#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HAL_GPIO_IGNITER = 0,
    HAL_GPIO_STARTER,
    HAL_GPIO_FUEL_ENABLE,
    HAL_GPIO_LED_STATUS,
    HAL_GPIO_COUNT,
} hal_gpio_pin_t;

typedef enum {
    HAL_GPIO_INPUT = 0,
    HAL_GPIO_OUTPUT,
} hal_gpio_dir_t;

int hal_gpio_init(void);
void hal_gpio_set_dir(hal_gpio_pin_t pin, hal_gpio_dir_t dir);
void hal_gpio_write(hal_gpio_pin_t pin, bool state);
bool hal_gpio_read(hal_gpio_pin_t pin);

#endif /* HAL_GPIO_H */
