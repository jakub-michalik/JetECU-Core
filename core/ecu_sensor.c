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

/* Individual validation steps */
static void check_range(ecu_sensor_reading_t *r, const ecu_sensor_config_t *cfg)
{
    if (r->value < cfg->range_min || r->value > cfg->range_max) {
        r->faults |= SENSOR_FAULT_RANGE;
        r->valid = false;
        r->value = clampf(r->value, cfg->range_min, cfg->range_max);
    }
}

static void check_rate(ecu_sensor_reading_t *r, const ecu_sensor_config_t *cfg,
                       float last_value)
{
    if (cfg->rate_limit <= 0.0f) return;

    float delta = r->value - last_value;
    if (fabsf_local(delta) > cfg->rate_limit) {
        r->faults |= SENSOR_FAULT_RATE;
        if (delta > 0.0f)
            r->value = last_value + cfg->rate_limit;
        else
            r->value = last_value - cfg->rate_limit;
    }
}

static void check_stuck(ecu_sensor_reading_t *r, const ecu_sensor_config_t *cfg,
                        ecu_sensor_state_t *state)
{
    if (cfg->stuck_count_limit <= 0) return;

    if (fabsf_local(r->value - state->last_value) < cfg->stuck_tolerance) {
        state->stuck_count++;
    } else {
        state->stuck_count = 0;
    }

    if (state->stuck_count >= cfg->stuck_count_limit) {
        r->faults |= SENSOR_FAULT_STUCK;
        r->valid = false;
    }
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

    check_range(&reading, cfg);

    if (!state->initialized) {
        state->last_value = reading.value;
        state->stuck_count = 0;
        state->initialized = true;
        return reading;
    }

    check_rate(&reading, cfg, state->last_value);
    check_stuck(&reading, cfg, state);

    state->last_value = reading.value;
    return reading;
}

bool ecu_sensor_egt_plausible(float egt, float rpm, float rpm_idle)
{
    if (rpm < rpm_idle * 0.5f && egt > 200.0f) {
        return false;
    }
    if (rpm > rpm_idle && egt < 50.0f) {
        return false;
    }
    return true;
}
