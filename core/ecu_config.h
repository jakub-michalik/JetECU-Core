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
    float egt_start_min;    /* minimum EGT to confirm ignition */

    /* Fuel */
    float fuel_start_pct;   /* fuel % during ignition */
    float fuel_max_pct;     /* absolute max fuel % */
    float fuel_ramp_rate;   /* %/sec ramp limit */

    /* Timeouts (ms) */
    uint32_t prestart_timeout_ms;
    uint32_t spinup_timeout_ms;
    uint32_t ignition_timeout_ms;

    /* RPM ramp */
    float rpm_ramp_rate;    /* RPM/sec during ramp */
} ecu_config_t;

/* Get default configuration */
void ecu_config_defaults(ecu_config_t *cfg);

#endif /* ECU_CONFIG_H */
