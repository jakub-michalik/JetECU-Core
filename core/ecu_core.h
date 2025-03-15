#ifndef ECU_CORE_H
#define ECU_CORE_H

#include "core/ecu_types.h"
#include "core/ecu_config.h"
#include "core/ecu_sm.h"
#include "core/ecu_sensor.h"
#include "core/ecu_pid.h"
#include "core/ecu_fuel.h"

/* Sensor inputs to the ECU step function */
typedef struct {
    float rpm;
    float egt;          /* degC */
    float throttle;     /* 0-100% */
} ecu_inputs_t;

/* Outputs from the ECU step function */
typedef struct {
    float fuel_pct;
    bool  igniter_on;
    bool  starter_on;
    ecu_state_t state;
    uint32_t fault_code;
} ecu_outputs_t;

/* Main ECU context */
typedef struct {
    ecu_sm_t           sm;
    ecu_config_t       config;
    ecu_pid_t          rpm_pid;
    ecu_fuel_state_t   fuel;

    ecu_sensor_state_t rpm_sensor;
    ecu_sensor_state_t egt_sensor;

    ecu_sensor_config_t rpm_sensor_cfg;
    ecu_sensor_config_t egt_sensor_cfg;

    uint32_t fault_code;
    ecu_time_t time_ms;
} ecu_t;

/* Initialize the ECU */
void ecu_init(ecu_t *ecu, const ecu_config_t *cfg);

/* Run one step. dt in seconds. Returns outputs. */
ecu_outputs_t ecu_step(ecu_t *ecu, const ecu_inputs_t *inputs, float dt);

#endif /* ECU_CORE_H */
