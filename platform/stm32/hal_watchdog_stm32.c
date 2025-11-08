#ifdef STM32F4
#include "hal/hal_watchdog.h"
#include "stm32f4xx_hal.h"

static IWDG_HandleTypeDef hiwdg;

int hal_watchdog_init(uint32_t timeout_ms)
{
    hiwdg.Instance = IWDG;

    /* IWDG clocked at ~32kHz (LSI) */
    /* Prescaler /64 -> 500Hz counter */
    /* Reload = timeout_ms * 500 / 1000 */
    hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
    hiwdg.Init.Reload = timeout_ms / 2;

    if (hiwdg.Init.Reload > 4095) hiwdg.Init.Reload = 4095;

    return (HAL_IWDG_Init(&hiwdg) == HAL_OK) ? 0 : -1;
}

void hal_watchdog_feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}
#endif /* STM32F4 */
