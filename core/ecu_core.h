/**
 * @file ecu_core.h
 * @brief Main ECU integration — ties together all core modules.
 *
 * Provides the top-level @c ecu_t context and the single-step control
 * loop that drives the state machine, sensor processing, PID, and fuel
 * scheduling on every iteration.
 */

#ifndef ECU_CORE_H
#define ECU_CORE_H

#include "core/ecu_types.h"
#include "core/ecu_config.h"
#include "core/ecu_sm.h"
#include "core/ecu_sensor.h"
#include "core/ecu_pid.h"
#include "core/ecu_fuel.h"
#include "core/ecu_map.h"

/**
 * @defgroup ecu_core Core ECU
 * @brief Top-level ECU context and control loop.
 * @{
 */

/** @brief Raw inputs fed into each control step. */
typedef struct {
    float rpm;        /**< Current shaft RPM. */
    float egt;        /**< Exhaust gas temperature (deg C). */
    float throttle;   /**< Throttle command 0–100 %. */
} ecu_inputs_t;

/** @brief Outputs computed by a single control step. */
typedef struct {
    float fuel_pct;      /**< Fuel valve command 0–100 %. */
    bool  igniter_on;    /**< Igniter relay state. */
    bool  starter_on;    /**< Starter motor relay state. */
    ecu_state_t state;   /**< Current state machine state. */
    uint32_t fault_code; /**< Active fault bitmask. */
} ecu_outputs_t;

/** @brief Complete ECU runtime context. */
typedef struct {
    ecu_sm_t           sm;             /**< State machine instance. */
    ecu_config_t       config;         /**< Active configuration. */
    ecu_pid_t          rpm_pid;        /**< RPM PID controller. */
    ecu_fuel_state_t   fuel;           /**< Fuel scheduling state. */

    ecu_sensor_state_t rpm_sensor;     /**< RPM sensor validation state. */
    ecu_sensor_state_t egt_sensor;     /**< EGT sensor validation state. */

    ecu_sensor_config_t rpm_sensor_cfg; /**< RPM sensor configuration. */
    ecu_sensor_config_t egt_sensor_cfg; /**< EGT sensor configuration. */

    /* Maps */
    ecu_map1d_t fuel_map;              /**< RPM → base fuel %. */
    ecu_map1d_t egt_limit_map;         /**< RPM → max EGT. */

    uint32_t fault_code;               /**< Accumulated fault bitmask. */
    ecu_time_t time_ms;                /**< Elapsed time in milliseconds. */
} ecu_t;

/**
 * @brief Initialise the ECU context with the given configuration.
 *
 * @param[out] ecu  ECU context to initialise.
 * @param[in]  cfg  Configuration parameters.
 */
void ecu_init(ecu_t *ecu, const ecu_config_t *cfg);

/**
 * @brief Execute one control-loop iteration.
 *
 * Reads sensors, updates the state machine, runs PID and fuel
 * scheduling, and returns the computed outputs.
 *
 * @param[in,out] ecu     ECU context.
 * @param[in]     inputs  Current sensor readings and throttle.
 * @param[in]     dt      Time step in seconds.
 * @return Computed outputs for this step.
 */
ecu_outputs_t ecu_step(ecu_t *ecu, const ecu_inputs_t *inputs, float dt);

/** @} */ /* end ecu_core */

#endif /* ECU_CORE_H */
