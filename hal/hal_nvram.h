#ifndef HAL_NVRAM_H
#define HAL_NVRAM_H

#include <stdint.h>
#include <stddef.h>

/* Initialize NVRAM / persistent storage */
int hal_nvram_init(void);

/* Read data from NVRAM. Returns bytes read, or -1 on error. */
int hal_nvram_read(const char *key, void *buf, size_t len);

/* Write data to NVRAM. Returns 0 on success. */
int hal_nvram_write(const char *key, const void *buf, size_t len);

/* Erase a key */
int hal_nvram_erase(const char *key);

#endif /* HAL_NVRAM_H */
