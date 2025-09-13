#ifndef ECU_CORE_H
#define ECU_CORE_H

#include "core/ecu_types.h"
#include "core/ecu_config.h"
#include "core/ecu_sm.h"
#include "core/ecu_sensor.h"
#include "core/ecu_pid.h"
#include "core/ecu_fuel.h"
#include "core/ecu_map.h"

typedef struct {
    float rpm;
    float egt;
    float throttle;
} ecu_inputs_t;

typedef struct {
    float fuel_pct;
    bool  igniter_on;
    bool  starter_on;
    ecu_state_t state;
    uint32_t fault_code;
} ecu_outputs_t;

typedef struct {
    ecu_sm_t           sm;
    ecu_config_t       config;
    ecu_pid_t          rpm_pid;
    ecu_fuel_state_t   fuel;

    ecu_sensor_state_t rpm_sensor;
    ecu_sensor_state_t egt_sensor;

    ecu_sensor_config_t rpm_sensor_cfg;
    ecu_sensor_config_t egt_sensor_cfg;

    /* Maps */
    ecu_map1d_t fuel_map;
    ecu_map1d_t egt_limit_map;

    uint32_t fault_code;
    ecu_time_t time_ms;
} ecu_t;

void ecu_init(ecu_t *ecu, const ecu_config_t *cfg);
ecu_outputs_t ecu_step(ecu_t *ecu, const ecu_inputs_t *inputs, float dt);

#endif /* ECU_CORE_H */
