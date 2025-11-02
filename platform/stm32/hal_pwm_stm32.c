#ifdef STM32F4
#include "hal/hal_pwm.h"
#include "stm32f4xx_hal.h"
#include "platform/stm32/stm32_hal_conf.h"

static TIM_HandleTypeDef htim3;

int hal_pwm_init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure PA6 as TIM3_CH1 */
    GPIO_InitTypeDef gpio = {
        .Pin = GPIO_PIN_6,
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Alternate = GPIO_AF2_TIM3,
    };
    HAL_GPIO_Init(GPIOA, &gpio);

    htim3.Instance = STM32_PWM_TIMER;
    htim3.Init.Prescaler = 83;         /* 84MHz / 84 = 1MHz */
    htim3.Init.Period = 19999;         /* 1MHz / 20000 = 50Hz */
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&htim3);

    TIM_OC_InitTypeDef oc = {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 1500,   /* 1.5ms default (center) */
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE,
    };
    HAL_TIM_PWM_ConfigChannel(&htim3, &oc, STM32_PWM_CHANNEL);
    HAL_TIM_PWM_Start(&htim3, STM32_PWM_CHANNEL);

    return 0;
}

void hal_pwm_set_duty(hal_pwm_channel_t ch, float duty_pct)
{
    (void)ch;
    uint32_t pulse = (uint32_t)(duty_pct / 100.0f * 19999.0f);
    __HAL_TIM_SET_COMPARE(&htim3, STM32_PWM_CHANNEL, pulse);
}

void hal_pwm_set_servo_us(hal_pwm_channel_t ch, uint16_t us)
{
    (void)ch;
    __HAL_TIM_SET_COMPARE(&htim3, STM32_PWM_CHANNEL, us);
}
#endif /* STM32F4 */
