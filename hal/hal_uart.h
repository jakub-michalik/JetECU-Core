#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    HAL_UART_0 = 0,
    HAL_UART_1,
    HAL_UART_COUNT,
} hal_uart_port_t;

typedef struct {
    uint32_t baudrate;
    uint8_t  data_bits;  /* 7 or 8 */
    uint8_t  stop_bits;  /* 1 or 2 */
    char     parity;     /* 'N', 'E', 'O' */
} hal_uart_config_t;

int hal_uart_init(hal_uart_port_t port, const hal_uart_config_t *cfg);

/* Write data. Returns bytes written. */
int hal_uart_write(hal_uart_port_t port, const uint8_t *data, size_t len);

/* Read data. Returns bytes read. Non-blocking. */
int hal_uart_read(hal_uart_port_t port, uint8_t *buf, size_t len);

/* Check if data is available */
int hal_uart_available(hal_uart_port_t port);

#endif /* HAL_UART_H */
