#ifdef STM32F4
#include "hal/hal_nvram.h"
#include "stm32f4xx_hal.h"
#include <string.h>

/*
 * Simple NVRAM using last flash sector.
 * For production, this should use a proper wear-leveling scheme.
 * For now: single sector with key-value pairs.
 */

#define NVRAM_SECTOR       FLASH_SECTOR_7
#define NVRAM_BASE_ADDR    0x08060000
#define NVRAM_SIZE         (128 * 1024)  /* 128KB sector */

/* Very simplified — stores a single blob at the sector start */
int hal_nvram_init(void)
{
    return 0;
}

int hal_nvram_read(const char *key, void *buf, size_t len)
{
    (void)key;
    /* Read from flash directly */
    memcpy(buf, (void *)NVRAM_BASE_ADDR, len);
    return (int)len;
}

int hal_nvram_write(const char *key, const void *buf, size_t len)
{
    (void)key;
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = NVRAM_SECTOR,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3,
    };
    uint32_t error;
    HAL_FLASHEx_Erase(&erase, &error);

    const uint8_t *src = (const uint8_t *)buf;
    for (size_t i = 0; i < len; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, NVRAM_BASE_ADDR + i, src[i]);
    }

    HAL_FLASH_Lock();
    return 0;
}

int hal_nvram_erase(const char *key)
{
    (void)key;
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = NVRAM_SECTOR,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3,
    };
    uint32_t error;
    HAL_FLASHEx_Erase(&erase, &error);
    HAL_FLASH_Lock();
    return 0;
}
#endif /* STM32F4 */
