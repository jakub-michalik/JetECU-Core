#ifndef ECU_CONFIG_H
#define ECU_CONFIG_H

#include "core/ecu_types.h"

typedef struct {
    /* RPM thresholds */
    float rpm_idle;
    float rpm_max;
    float rpm_start_target;

    /* EGT limits (degC) */
    float egt_max;
    float egt_start_min;

    /* Fuel */
    float fuel_start_pct;
    float fuel_max_pct;
    float fuel_ramp_rate;   /* %/sec ramp limit */

    /* Timeouts (ms) */
    uint32_t prestart_timeout_ms;
    uint32_t spinup_timeout_ms;
    uint32_t ignition_timeout_ms;

    /* RPM ramp */
    float rpm_ramp_rate;
} ecu_config_t;

/* Get default configuration */
void ecu_config_defaults(ecu_config_t *cfg);

#endif /* ECU_CONFIG_H */
