#ifdef STM32F4
#include "hal/hal_system.h"
#include "stm32f4xx_hal.h"

int hal_system_init(void)
{
    HAL_Init();

    /* Configure system clock to 84MHz (HSI + PLL) */
    RCC_OscInitTypeDef osc = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSI,
        .HSIState = RCC_HSI_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSI,
        .PLL.PLLM = 8,
        .PLL.PLLN = 168,
        .PLL.PLLP = RCC_PLLP_DIV4,
        .PLL.PLLQ = 7,
    };
    HAL_RCC_OscConfig(&osc);

    RCC_ClkInitTypeDef clk = {
        .ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                     RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
        .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
        .AHBCLKDivider = RCC_SYSCLK_DIV1,
        .APB1CLKDivider = RCC_HCLK_DIV2,
        .APB2CLKDivider = RCC_HCLK_DIV1,
    };
    HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2);

    return 0;
}

void hal_system_deinit(void)
{
    HAL_DeInit();
}

void hal_system_reset(void)
{
    NVIC_SystemReset();
}

uint32_t hal_system_get_id(void)
{
    return HAL_GetUIDw0();
}

uint32_t hal_system_get_clock_hz(void)
{
    return HAL_RCC_GetHCLKFreq();
}
#endif /* STM32F4 */
