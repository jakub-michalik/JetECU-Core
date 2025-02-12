#include "core/ecu_sensor.h"

void ecu_sensor_init(ecu_sensor_state_t *state)
{
    state->last_value = 0.0f;
    state->stuck_count = 0;
    state->initialized = false;
}

static float clampf(float val, float lo, float hi)
{
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

static float fabsf_local(float x)
{
    return x < 0.0f ? -x : x;
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
        reading.value = clampf(raw, cfg->range_min, cfg->range_max);
    }

    if (!state->initialized) {
        state->last_value = reading.value;
        state->stuck_count = 0;
        state->initialized = true;
        return reading;
    }

    /* Rate-of-change limiting */
    if (cfg->rate_limit > 0.0f) {
        float delta = reading.value - state->last_value;
        if (fabsf_local(delta) > cfg->rate_limit) {
            reading.faults |= SENSOR_FAULT_RATE;
            if (delta > 0.0f)
                reading.value = state->last_value + cfg->rate_limit;
            else
                reading.value = state->last_value - cfg->rate_limit;
        }
    }

    /* Stuck-at detection — only after we have a previous reading */
    if (cfg->stuck_count_limit > 0) {
        if (fabsf_local(reading.value - state->last_value) < cfg->stuck_tolerance) {
            state->stuck_count++;
        } else {
            state->stuck_count = 0;
        }

        if (state->stuck_count >= cfg->stuck_count_limit) {
            reading.faults |= SENSOR_FAULT_STUCK;
            reading.valid = false;
        }
    }

    state->last_value = reading.value;
    return reading;
}
