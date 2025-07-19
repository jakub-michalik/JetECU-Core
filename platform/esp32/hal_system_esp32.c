#ifdef ESP_PLATFORM
#include "hal/hal_system.h"
#include "esp_system.h"
#include "esp_chip_info.h"

int hal_system_init(void)
{
    /* Board-level init handled by ESP-IDF before app_main */
    return 0;
}

void hal_system_deinit(void) {}

void hal_system_reset(void)
{
    esp_restart();
}

uint32_t hal_system_get_id(void)
{
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    return (uint32_t)(mac[2] << 24 | mac[3] << 16 | mac[4] << 8 | mac[5]);
}

uint32_t hal_system_get_clock_hz(void)
{
    return 240000000;  /* ESP32 default 240MHz */
}
#endif /* ESP_PLATFORM */
