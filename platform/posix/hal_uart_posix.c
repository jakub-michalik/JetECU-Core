#include "hal/hal_uart.h"
#include <stdio.h>
#include <string.h>

/* POSIX UART: write to stdout, read from stdin (non-blocking not implemented) */

int hal_uart_init(hal_uart_port_t port, const hal_uart_config_t *cfg)
{
    (void)port;
    (void)cfg;
    return 0;
}

int hal_uart_write(hal_uart_port_t port, const uint8_t *data, size_t len)
{
    (void)port;
    return (int)fwrite(data, 1, len, stdout);
}

int hal_uart_read(hal_uart_port_t port, uint8_t *buf, size_t len)
{
    (void)port;
    (void)buf;
    (void)len;
    return 0;  /* non-blocking: nothing available */
}

int hal_uart_available(hal_uart_port_t port)
{
    (void)port;
    return 0;
}
