#ifdef ESP_PLATFORM
#include "hal/hal_timer.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int hal_timer_init(void) { return 0; }

uint32_t hal_timer_millis(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

uint32_t hal_timer_micros(void)
{
    return (uint32_t)esp_timer_get_time();
}

void hal_timer_delay_ms(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}
#endif /* ESP_PLATFORM */
