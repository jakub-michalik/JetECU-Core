#ifndef SIM_ENGINE_H
#define SIM_ENGINE_H

/*
 * Simple thermodynamic engine model for simulation.
 * Models RPM response to fuel flow, and EGT from combustion.
 */

typedef struct {
    float rpm;          /* current RPM */
    float egt;          /* exhaust gas temp (degC) */
    float fuel_flow;    /* current fuel input (0-100%) */
    float ambient_temp; /* ambient temperature */

    /* Model parameters */
    float inertia;      /* rotational inertia factor */
    float drag;         /* aerodynamic drag coefficient */
    float combustion_eff; /* combustion efficiency */
    float thermal_mass;   /* thermal mass for EGT */
    float cooling_rate;   /* EGT cooling rate */
} sim_engine_t;

/* Initialize engine model with defaults */
void sim_engine_init(sim_engine_t *eng);

/* Step the model. dt in seconds. */
void sim_engine_step(sim_engine_t *eng, float fuel_pct, float starter_torque, float dt);

#endif /* SIM_ENGINE_H */
