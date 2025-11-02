#ifdef STM32F4
#include "hal/hal_timer.h"
#include "stm32f4xx_hal.h"

int hal_timer_init(void)
{
    /* SysTick already configured by HAL_Init */
    return 0;
}

uint32_t hal_timer_millis(void)
{
    return HAL_GetTick();
}

uint32_t hal_timer_micros(void)
{
    /* Use DWT cycle counter for microsecond resolution */
    return HAL_GetTick() * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000);
}

void hal_timer_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
#endif /* STM32F4 */
