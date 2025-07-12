#ifdef ESP_PLATFORM
#include "hal/hal_gpio.h"
#include "driver/gpio.h"

static const gpio_num_t pin_map[] = {
    [HAL_GPIO_IGNITER]     = GPIO_NUM_25,
    [HAL_GPIO_STARTER]     = GPIO_NUM_26,
    [HAL_GPIO_FUEL_ENABLE] = GPIO_NUM_27,
    [HAL_GPIO_LED_STATUS]  = GPIO_NUM_2,   /* onboard LED */
};

int hal_gpio_init(void)
{
    for (int i = 0; i < HAL_GPIO_COUNT; i++) {
        gpio_reset_pin(pin_map[i]);
        gpio_set_direction(pin_map[i], GPIO_MODE_OUTPUT);
        gpio_set_level(pin_map[i], 0);
    }
    return 0;
}

void hal_gpio_set_dir(hal_gpio_pin_t pin, hal_gpio_dir_t dir)
{
    if (pin >= HAL_GPIO_COUNT) return;
    gpio_set_direction(pin_map[pin],
        dir == HAL_GPIO_OUTPUT ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
}

void hal_gpio_write(hal_gpio_pin_t pin, bool state)
{
    if (pin >= HAL_GPIO_COUNT) return;
    gpio_set_level(pin_map[pin], state ? 1 : 0);
}

bool hal_gpio_read(hal_gpio_pin_t pin)
{
    if (pin >= HAL_GPIO_COUNT) return false;
    return gpio_get_level(pin_map[pin]) != 0;
}
#endif /* ESP_PLATFORM */
