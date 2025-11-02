#ifdef STM32F4
#include "hal/hal_adc.h"
#include "stm32f4xx_hal.h"
#include "platform/stm32/stm32_hal_conf.h"

static ADC_HandleTypeDef hadc1;

static const uint32_t channel_map[] = {
    [HAL_ADC_CH_RPM]  = STM32_ADC_RPM_CH,
    [HAL_ADC_CH_EGT]  = STM32_ADC_EGT_CH,
    [HAL_ADC_CH_VBAT] = STM32_ADC_VBAT_CH,
};

int hal_adc_init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    hadc1.Instance = ADC1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;

    return (HAL_ADC_Init(&hadc1) == HAL_OK) ? 0 : -1;
}

uint16_t hal_adc_read_raw(hal_adc_channel_t ch)
{
    if (ch >= HAL_ADC_CH_COUNT) return 0;

    ADC_ChannelConfTypeDef cfg = {
        .Channel = channel_map[ch],
        .Rank = 1,
        .SamplingTime = ADC_SAMPLETIME_84CYCLES,
    };
    HAL_ADC_ConfigChannel(&hadc1, &cfg);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t val = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return val;
}

float hal_adc_read_voltage(hal_adc_channel_t ch)
{
    return (float)hal_adc_read_raw(ch) / 4095.0f * 3.3f;
}
#endif /* STM32F4 */
