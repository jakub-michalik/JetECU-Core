#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

typedef enum {
    HAL_ADC_CH_RPM = 0,
    HAL_ADC_CH_EGT,
    HAL_ADC_CH_VBAT,
    HAL_ADC_CH_COUNT,
} hal_adc_channel_t;

/* Initialize ADC peripheral */
int hal_adc_init(void);

/* Read raw ADC value (0-4095 for 12-bit) */
uint16_t hal_adc_read_raw(hal_adc_channel_t ch);

/* Read voltage (0.0 - 3.3V typical) */
float hal_adc_read_voltage(hal_adc_channel_t ch);

#endif /* HAL_ADC_H */
