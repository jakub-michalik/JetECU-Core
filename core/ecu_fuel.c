#include "core/ecu_fuel.h"

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

void ecu_fuel_init(ecu_fuel_state_t *state)
{
    state->last_pct = 0.0f;
    state->cutoff_active = false;
}

ecu_fuel_output_t ecu_fuel_compute(
    float target_pct,
    float dt,
    const ecu_config_t *cfg,
    ecu_fuel_state_t *state)
{
    ecu_fuel_output_t out;
    out.cutoff = state->cutoff_active;

    if (state->cutoff_active) {
        out.command_pct = target_pct;
        out.actual_pct = 0.0f;
        state->last_pct = 0.0f;
        return out;
    }

    /* Clamp to max */
    target_pct = clampf(target_pct, 0.0f, cfg->fuel_max_pct);
    out.command_pct = target_pct;

    /* Rate limiting */
    float max_change = cfg->fuel_ramp_rate * dt;
    float delta = target_pct - state->last_pct;

    if (fabsf_local(delta) > max_change) {
        if (delta > 0.0f)
            target_pct = state->last_pct + max_change;
        else
            target_pct = state->last_pct - max_change;
    }

    out.actual_pct = clampf(target_pct, 0.0f, cfg->fuel_max_pct);
    state->last_pct = out.actual_pct;
    return out;
}

void ecu_fuel_cutoff(ecu_fuel_state_t *state)
{
    state->cutoff_active = true;
    state->last_pct = 0.0f;
}

void ecu_fuel_release(ecu_fuel_state_t *state)
{
    state->cutoff_active = false;
}
