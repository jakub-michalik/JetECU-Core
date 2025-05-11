#include "sim/sim_engine.h"

void sim_engine_init(sim_engine_t *eng)
{
    eng->rpm = 0.0f;
    eng->egt = 25.0f;
    eng->fuel_flow = 0.0f;
    eng->ambient_temp = 25.0f;

    eng->inertia = 0.005f;    /* increased inertia */
    eng->drag = 0.000005f;
    eng->combustion_eff = 0.7f;
    eng->thermal_mass = 5.0f;
    eng->cooling_rate = 0.03f;
}

void sim_engine_step(sim_engine_t *eng, float fuel_pct, float starter_torque, float dt)
{
    eng->fuel_flow = fuel_pct;

    float combustion_torque = 0.0f;
    if (eng->egt > 120.0f && fuel_pct > 0.5f) {
        combustion_torque = fuel_pct * eng->combustion_eff * 5.0f;
    }

    float drag_torque = eng->drag * eng->rpm * eng->rpm;

    float net_torque = combustion_torque + starter_torque - drag_torque;
    float rpm_delta = (net_torque / eng->inertia) * dt;
    eng->rpm += rpm_delta;
    if (eng->rpm < 0.0f) eng->rpm = 0.0f;

    /* EGT model — more gradual */
    float heat_input = 0.0f;
    if (fuel_pct > 0.5f) {
        heat_input = fuel_pct * eng->combustion_eff * 12.0f;
    }
    float heat_loss = eng->cooling_rate * (eng->egt - eng->ambient_temp);
    float airflow_cooling = 0.0f;
    if (eng->rpm > 100.0f) {
        airflow_cooling = (eng->rpm / 100000.0f) * 0.1f * (eng->egt - eng->ambient_temp);
    }

    float egt_delta = (heat_input - heat_loss - airflow_cooling) / eng->thermal_mass * dt;
    eng->egt += egt_delta;
    if (eng->egt < eng->ambient_temp) eng->egt = eng->ambient_temp;
}
