#ifdef ESP_PLATFORM
#include "hal/hal_nvram.h"
#include "nvs_flash.h"
#include "nvs.h"

#define NVS_NAMESPACE "jetecu"

int hal_nvram_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    return (err == ESP_OK) ? 0 : -1;
}

int hal_nvram_read(const char *key, void *buf, size_t len)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) return -1;
    size_t actual = len;
    esp_err_t err = nvs_get_blob(h, key, buf, &actual);
    nvs_close(h);
    return (err == ESP_OK) ? (int)actual : -1;
}

int hal_nvram_write(const char *key, const void *buf, size_t len)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) return -1;
    esp_err_t err = nvs_set_blob(h, key, buf, len);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);
    return (err == ESP_OK) ? 0 : -1;
}

int hal_nvram_erase(const char *key)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) return -1;
    esp_err_t err = nvs_erase_key(h, key);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);
    return (err == ESP_OK) ? 0 : -1;
}
#endif /* ESP_PLATFORM */
