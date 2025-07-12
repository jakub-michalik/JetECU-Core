#ifdef ESP_PLATFORM
#include "hal/hal_adc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

static esp_adc_cal_characteristics_t adc_chars;
static const adc1_channel_t channel_map[] = {
    [HAL_ADC_CH_RPM] = ADC1_CHANNEL_6,  /* GPIO34 */
    [HAL_ADC_CH_EGT] = ADC1_CHANNEL_7,  /* GPIO35 */
    [HAL_ADC_CH_VBAT] = ADC1_CHANNEL_4, /* GPIO32 */
};

int hal_adc_init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    for (int i = 0; i < HAL_ADC_CH_COUNT; i++) {
        adc1_config_channel_atten(channel_map[i], ADC_ATTEN_DB_11);
    }
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11,
                              ADC_WIDTH_BIT_12, 1100, &adc_chars);
    return 0;
}

uint16_t hal_adc_read_raw(hal_adc_channel_t ch)
{
    if (ch >= HAL_ADC_CH_COUNT) return 0;
    return (uint16_t)adc1_get_raw(channel_map[ch]);
}

float hal_adc_read_voltage(hal_adc_channel_t ch)
{
    if (ch >= HAL_ADC_CH_COUNT) return 0.0f;
    uint32_t mv;
    esp_adc_cal_get_voltage(channel_map[ch], &adc_chars, &mv);
    return (float)mv / 1000.0f;
}
#endif /* ESP_PLATFORM */
