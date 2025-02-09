#ifndef ECU_CONFIG_LOAD_H
#define ECU_CONFIG_LOAD_H

#include "core/ecu_config.h"

/*
 * Load config from JSON file. Falls back to defaults for missing keys.
 * Returns ECU_OK on success, ECU_ERR_CONFIG on parse error.
 */
ecu_err_t ecu_config_load_file(const char *path, ecu_config_t *cfg);

/* Load config from JSON string */
ecu_err_t ecu_config_load_str(const char *json, ecu_config_t *cfg);

#endif /* ECU_CONFIG_LOAD_H */
