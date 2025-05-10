#include "hal/hal_gpio.h"

static bool gpio_state[HAL_GPIO_COUNT] = {false};
static hal_gpio_dir_t gpio_dir[HAL_GPIO_COUNT] = {HAL_GPIO_INPUT};

int hal_gpio_init(void) { return 0; }

void hal_gpio_set_dir(hal_gpio_pin_t pin, hal_gpio_dir_t dir)
{
    if (pin < HAL_GPIO_COUNT) gpio_dir[pin] = dir;
}

void hal_gpio_write(hal_gpio_pin_t pin, bool state)
{
    if (pin < HAL_GPIO_COUNT) gpio_state[pin] = state;
}

bool hal_gpio_read(hal_gpio_pin_t pin)
{
    if (pin < HAL_GPIO_COUNT) return gpio_state[pin];
    return false;
}
