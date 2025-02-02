#include "core/ecu_sensor.h"

void ecu_sensor_init(ecu_sensor_state_t *state)
{
    state->last_value = 0.0f;
    state->stuck_count = 0;
    state->initialized = false;
}

ecu_sensor_reading_t ecu_sensor_validate(
    float raw,
    const ecu_sensor_config_t *cfg,
    ecu_sensor_state_t *state)
{
    ecu_sensor_reading_t reading = {
        .raw = raw,
        .value = raw,
        .valid = true,
        .faults = SENSOR_FAULT_NONE,
    };

    /* Range check */
    if (raw < cfg->range_min || raw > cfg->range_max) {
        reading.faults |= SENSOR_FAULT_RANGE;
        reading.valid = false;
        /* clamp to range */
        if (raw < cfg->range_min) reading.value = cfg->range_min;
        if (raw > cfg->range_max) reading.value = cfg->range_max;
    }

    if (!state->initialized) {
        state->last_value = reading.value;
        state->initialized = true;
        return reading;
    }

    state->last_value = reading.value;
    return reading;
}
