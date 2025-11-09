#ifdef STM32F4
#include "hal/hal_uart.h"
#include "stm32f4xx_hal.h"

static UART_HandleTypeDef huart[HAL_UART_COUNT];
static USART_TypeDef *uart_instances[] = {
    [HAL_UART_0] = USART1,
    [HAL_UART_1] = USART2,
};

int hal_uart_init(hal_uart_port_t port, const hal_uart_config_t *cfg)
{
    if (port >= HAL_UART_COUNT) return -1;

    if (port == HAL_UART_0) __HAL_RCC_USART1_CLK_ENABLE();
    if (port == HAL_UART_1) __HAL_RCC_USART2_CLK_ENABLE();

    huart[port].Instance = uart_instances[port];
    huart[port].Init.BaudRate = cfg->baudrate;
    huart[port].Init.WordLength = (cfg->data_bits == 7) ? UART_WORDLENGTH_7B : UART_WORDLENGTH_8B;
    huart[port].Init.StopBits = (cfg->stop_bits == 2) ? UART_STOPBITS_2 : UART_STOPBITS_1;
    huart[port].Init.Parity = UART_PARITY_NONE;
    huart[port].Init.Mode = UART_MODE_TX_RX;
    huart[port].Init.HwFlowCtl = UART_HWCONTROL_NONE;

    return (HAL_UART_Init(&huart[port]) == HAL_OK) ? 0 : -1;
}

int hal_uart_write(hal_uart_port_t port, const uint8_t *data, size_t len)
{
    if (port >= HAL_UART_COUNT) return -1;
    HAL_StatusTypeDef s = HAL_UART_Transmit(&huart[port], data, (uint16_t)len, 100);
    return (s == HAL_OK) ? (int)len : -1;
}

int hal_uart_read(hal_uart_port_t port, uint8_t *buf, size_t len)
{
    if (port >= HAL_UART_COUNT) return -1;
    HAL_StatusTypeDef s = HAL_UART_Receive(&huart[port], buf, (uint16_t)len, 1);
    return (s == HAL_OK) ? (int)len : 0;
}

int hal_uart_available(hal_uart_port_t port)
{
    if (port >= HAL_UART_COUNT) return 0;
    return __HAL_UART_GET_FLAG(&huart[port], UART_FLAG_RXNE) ? 1 : 0;
}
#endif /* STM32F4 */
