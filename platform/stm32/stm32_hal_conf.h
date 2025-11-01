#ifndef STM32_HAL_CONF_H
#define STM32_HAL_CONF_H

/*
 * STM32 HAL configuration for JetECU.
 * Target: STM32F4xx (e.g., STM32F411, STM32F405)
 *
 * This file is referenced by the STM32 HAL drivers.
 * Pin assignments and peripheral config are defined per-board.
 */

/* ADC channels */
#define STM32_ADC_RPM_CH     ADC_CHANNEL_0   /* PA0 */
#define STM32_ADC_EGT_CH     ADC_CHANNEL_1   /* PA1 */
#define STM32_ADC_VBAT_CH    ADC_CHANNEL_4   /* PA4 */

/* GPIO pins */
#define STM32_PIN_IGNITER    GPIO_PIN_5      /* PB5 */
#define STM32_PORT_IGNITER   GPIOB
#define STM32_PIN_STARTER    GPIO_PIN_6      /* PB6 */
#define STM32_PORT_STARTER   GPIOB
#define STM32_PIN_FUEL_EN    GPIO_PIN_7      /* PB7 */
#define STM32_PORT_FUEL_EN   GPIOB
#define STM32_PIN_LED        GPIO_PIN_13     /* PC13 */
#define STM32_PORT_LED       GPIOC

/* PWM */
#define STM32_PWM_TIMER      TIM3
#define STM32_PWM_CHANNEL    TIM_CHANNEL_1   /* PA6 */

/* UART */
#define STM32_UART_TELEMETRY USART2
#define STM32_UART_BAUD      115200

/* CAN */
#define STM32_CAN            CAN1

#endif /* STM32_HAL_CONF_H */
