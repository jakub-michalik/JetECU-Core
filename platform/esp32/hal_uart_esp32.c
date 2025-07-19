#ifdef ESP_PLATFORM
#include "hal/hal_uart.h"
#include "driver/uart.h"

static const uart_port_t port_map[] = {
    [HAL_UART_0] = UART_NUM_0,
    [HAL_UART_1] = UART_NUM_1,
};

#define UART_BUF_SIZE 256

int hal_uart_init(hal_uart_port_t port, const hal_uart_config_t *cfg)
{
    if (port >= HAL_UART_COUNT) return -1;

    uart_config_t ucfg = {
        .baud_rate = (int)cfg->baudrate,
        .data_bits = (cfg->data_bits == 7) ? UART_DATA_7_BITS : UART_DATA_8_BITS,
        .parity = (cfg->parity == 'E') ? UART_PARITY_EVEN :
                  (cfg->parity == 'O') ? UART_PARITY_ODD : UART_PARITY_DISABLE,
        .stop_bits = (cfg->stop_bits == 2) ? UART_STOP_BITS_2 : UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_param_config(port_map[port], &ucfg);
    uart_driver_install(port_map[port], UART_BUF_SIZE, UART_BUF_SIZE, 0, NULL, 0);
    return 0;
}

int hal_uart_write(hal_uart_port_t port, const uint8_t *data, size_t len)
{
    if (port >= HAL_UART_COUNT) return -1;
    return uart_write_bytes(port_map[port], data, len);
}

int hal_uart_read(hal_uart_port_t port, uint8_t *buf, size_t len)
{
    if (port >= HAL_UART_COUNT) return -1;
    return uart_read_bytes(port_map[port], buf, len, 0);
}

int hal_uart_available(hal_uart_port_t port)
{
    if (port >= HAL_UART_COUNT) return 0;
    size_t avail = 0;
    uart_get_buffered_data_len(port_map[port], &avail);
    return (int)avail;
}
#endif /* ESP_PLATFORM */
