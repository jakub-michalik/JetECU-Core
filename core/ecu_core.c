#include "core/ecu_core.h"

void ecu_init(ecu_t *ecu, const ecu_config_t *cfg)
{
    ecu->config = *cfg;
    ecu_sm_init(&ecu->sm);
    ecu_fuel_init(&ecu->fuel);
    ecu->fault_code = 0;
    ecu->time_ms = 0;

    ecu_pid_config_t pid_cfg = {
        .kp = cfg->pid_kp,
        .ki = cfg->pid_ki,
        .kd = cfg->pid_kd,
        .output_min = 0.0f,
        .output_max = cfg->fuel_max_pct,
        .integral_max = 0.0f,
    };
    ecu_pid_init(&ecu->rpm_pid, &pid_cfg);

    ecu->rpm_sensor_cfg = (ecu_sensor_config_t){
        .range_min = 0.0f,
        .range_max = cfg->rpm_max * 1.2f,
        .rate_limit = 5000.0f,
        .stuck_tolerance = 10.0f,
        .stuck_count_limit = 100,
    };
    ecu->egt_sensor_cfg = (ecu_sensor_config_t){
        .range_min = -50.0f,
        .range_max = cfg->egt_max * 1.2f,
        .rate_limit = 100.0f,
        .stuck_tolerance = 0.5f,
        .stuck_count_limit = 200,
    };

    ecu_sensor_init(&ecu->rpm_sensor);
    ecu_sensor_init(&ecu->egt_sensor);
}

static void enter_fault(ecu_t *ecu, uint32_t code)
{
    ecu->fault_code |= code;
    ecu_sm_transition(&ecu->sm, ECU_STATE_FAULT, ecu->time_ms);
    ecu_fuel_cutoff(&ecu->fuel);
}

ecu_outputs_t ecu_step(ecu_t *ecu, const ecu_inputs_t *inputs, float dt)
{
    ecu_outputs_t out = {0};
    ecu->time_ms += (uint32_t)(dt * 1000.0f);

    ecu_sensor_reading_t rpm = ecu_sensor_validate(
        inputs->rpm, &ecu->rpm_sensor_cfg, &ecu->rpm_sensor);
    ecu_sensor_reading_t egt = ecu_sensor_validate(
        inputs->egt, &ecu->egt_sensor_cfg, &ecu->egt_sensor);

    ecu_state_t state = ecu_sm_state(&ecu->sm);

    switch (state) {
    case ECU_STATE_OFF:
        out.fuel_pct = 0.0f;
        out.igniter_on = false;
        out.starter_on = false;
        if (inputs->throttle > 5.0f) {
            ecu_sm_transition(&ecu->sm, ECU_STATE_PRESTART, ecu->time_ms);
        }
        break;

    case ECU_STATE_PRESTART:
        out.fuel_pct = 0.0f;
        out.starter_on = false;
        /* Sensor self-check: need at least one valid reading before proceeding */
        if (!rpm.valid || !egt.valid) {
            if (ecu->time_ms - ecu->sm.state_enter_time > ecu->config.prestart_timeout_ms) {
                enter_fault(ecu, 0x01);
            }
            break;
        }
        ecu_sm_transition(&ecu->sm, ECU_STATE_SPINUP, ecu->time_ms);
        break;

    case ECU_STATE_SPINUP:
        out.starter_on = true;
        out.fuel_pct = 0.0f;
        if (rpm.value >= ecu->config.rpm_idle * 0.3f) {
            ecu_sm_transition(&ecu->sm, ECU_STATE_IGNITION, ecu->time_ms);
        }
        if (ecu->time_ms - ecu->sm.state_enter_time > ecu->config.spinup_timeout_ms) {
            enter_fault(ecu, 0x04);
        }
        break;

    case ECU_STATE_IGNITION: {
        out.starter_on = true;
        out.igniter_on = true;
        ecu_fuel_output_t fuel = ecu_fuel_compute(
            ecu->config.fuel_start_pct, dt, &ecu->config, &ecu->fuel);
        out.fuel_pct = fuel.actual_pct;

        /* Check EGT rise confirms ignition */
        if (egt.value >= ecu->config.egt_start_min) {
            ecu_sm_transition(&ecu->sm, ECU_STATE_RAMP, ecu->time_ms);
        }
        if (ecu->time_ms - ecu->sm.state_enter_time > ecu->config.ignition_timeout_ms) {
            enter_fault(ecu, 0x08);
        }
        break;
    }

    case ECU_STATE_RAMP: {
        out.starter_on = false;
        out.igniter_on = false;
        float target_rpm = ecu->config.rpm_start_target;
        float fuel_cmd = ecu_pid_update(&ecu->rpm_pid, target_rpm, rpm.value, dt);
        ecu_fuel_output_t fuel = ecu_fuel_compute(fuel_cmd, dt, &ecu->config, &ecu->fuel);
        out.fuel_pct = fuel.actual_pct;

        if (rpm.value >= ecu->config.rpm_start_target * 0.95f) {
            ecu_sm_transition(&ecu->sm, ECU_STATE_RUN, ecu->time_ms);
        }
        break;
    }

    case ECU_STATE_RUN: {
        out.starter_on = false;
        out.igniter_on = false;
        float target_rpm = ecu->config.rpm_idle +
            (ecu->config.rpm_max - ecu->config.rpm_idle) * (inputs->throttle / 100.0f);
        float fuel_cmd = ecu_pid_update(&ecu->rpm_pid, target_rpm, rpm.value, dt);
        ecu_fuel_output_t fuel = ecu_fuel_compute(fuel_cmd, dt, &ecu->config, &ecu->fuel);
        out.fuel_pct = fuel.actual_pct;

        if (inputs->throttle < 1.0f) {
            ecu_sm_transition(&ecu->sm, ECU_STATE_COOLDOWN, ecu->time_ms);
        }
        break;
    }

    case ECU_STATE_COOLDOWN:
        out.fuel_pct = 0.0f;
        ecu_fuel_cutoff(&ecu->fuel);
        if (egt.value < 100.0f) {
            ecu_sm_transition(&ecu->sm, ECU_STATE_SHUTDOWN, ecu->time_ms);
        }
        break;

    case ECU_STATE_SHUTDOWN:
        out.fuel_pct = 0.0f;
        out.starter_on = false;
        out.igniter_on = false;
        ecu_sm_transition(&ecu->sm, ECU_STATE_OFF, ecu->time_ms);
        break;

    case ECU_STATE_FAULT:
        out.fuel_pct = 0.0f;
        out.igniter_on = false;
        out.starter_on = false;
        ecu_fuel_cutoff(&ecu->fuel);
        break;
    }

    out.state = ecu_sm_state(&ecu->sm);
    out.fault_code = ecu->fault_code;
    return out;
}
