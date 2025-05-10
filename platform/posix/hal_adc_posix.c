#include "hal/hal_adc.h"

/* POSIX stub — values are fed by simulator */
static float adc_values[HAL_ADC_CH_COUNT] = {0};

int hal_adc_init(void) { return 0; }

uint16_t hal_adc_read_raw(hal_adc_channel_t ch)
{
    if (ch >= HAL_ADC_CH_COUNT) return 0;
    return (uint16_t)(adc_values[ch] / 3.3f * 4095.0f);
}

float hal_adc_read_voltage(hal_adc_channel_t ch)
{
    if (ch >= HAL_ADC_CH_COUNT) return 0.0f;
    return adc_values[ch];
}

/* Simulator API: set ADC channel value */
void hal_adc_posix_set(hal_adc_channel_t ch, float voltage)
{
    if (ch < HAL_ADC_CH_COUNT) {
        adc_values[ch] = voltage;
    }
}
