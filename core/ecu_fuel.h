#ifndef ECU_FUEL_H
#define ECU_FUEL_H

#include "core/ecu_types.h"
#include "core/ecu_config.h"

/* Fuel command (0-100% valve position) */
typedef struct {
    float command_pct;    /* requested fuel % */
    float actual_pct;     /* after rate limiting */
    bool  cutoff;         /* safety cutoff active */
} ecu_fuel_output_t;

typedef struct {
    float last_pct;
    bool  cutoff_active;
} ecu_fuel_state_t;

void ecu_fuel_init(ecu_fuel_state_t *state);

/* Compute fuel output given target and config */
ecu_fuel_output_t ecu_fuel_compute(
    float target_pct,
    float dt,
    const ecu_config_t *cfg,
    ecu_fuel_state_t *state
);

/* Emergency fuel cutoff */
void ecu_fuel_cutoff(ecu_fuel_state_t *state);

/* Release cutoff */
void ecu_fuel_release(ecu_fuel_state_t *state);

#endif /* ECU_FUEL_H */
