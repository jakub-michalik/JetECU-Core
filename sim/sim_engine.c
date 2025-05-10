#include "sim/sim_engine.h"

void sim_engine_init(sim_engine_t *eng)
{
    eng->rpm = 0.0f;
    eng->egt = 25.0f;
    eng->fuel_flow = 0.0f;
    eng->ambient_temp = 25.0f;

    eng->inertia = 0.001f;
    eng->drag = 0.00001f;
    eng->combustion_eff = 0.8f;
    eng->thermal_mass = 2.0f;
    eng->cooling_rate = 0.05f;
}

void sim_engine_step(sim_engine_t *eng, float fuel_pct, float starter_torque, float dt)
{
    eng->fuel_flow = fuel_pct;

    /* Combustion produces torque proportional to fuel flow and RPM */
    float combustion_torque = 0.0f;
    if (eng->egt > 100.0f && fuel_pct > 1.0f) {
        /* Engine is lit — fuel produces thrust */
        combustion_torque = fuel_pct * eng->combustion_eff * 10.0f;
    }

    /* Drag torque increases with RPM^2 */
    float drag_torque = eng->drag * eng->rpm * eng->rpm;

    /* Net torque */
    float net_torque = combustion_torque + starter_torque - drag_torque;

    /* RPM change: torque / inertia * dt */
    float rpm_delta = (net_torque / eng->inertia) * dt;
    eng->rpm += rpm_delta;
    if (eng->rpm < 0.0f) eng->rpm = 0.0f;

    /* EGT model */
    float heat_input = fuel_pct * eng->combustion_eff * 15.0f;
    float heat_loss = eng->cooling_rate * (eng->egt - eng->ambient_temp);

    /* Air flow cooling increases with RPM */
    float airflow_cooling = eng->rpm * 0.00005f * (eng->egt - eng->ambient_temp);

    float egt_delta = (heat_input - heat_loss - airflow_cooling) / eng->thermal_mass * dt;
    eng->egt += egt_delta;
    if (eng->egt < eng->ambient_temp) eng->egt = eng->ambient_temp;
}
