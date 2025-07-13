#ifdef ESP_PLATFORM
#include "hal/hal_watchdog.h"
#include "esp_task_wdt.h"

int hal_watchdog_init(uint32_t timeout_ms)
{
    esp_task_wdt_config_t cfg = {
        .timeout_ms = timeout_ms,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    esp_task_wdt_reconfigure(&cfg);
    esp_task_wdt_add(NULL);
    return 0;
}

void hal_watchdog_feed(void)
{
    esp_task_wdt_reset();
}
#endif /* ESP_PLATFORM */
