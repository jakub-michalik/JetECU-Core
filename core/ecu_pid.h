/**
 * @file ecu_pid.h
 * @brief PID controller with anti-windup.
 */

#ifndef ECU_PID_H
#define ECU_PID_H

#include <stdbool.h>

/**
 * @defgroup ecu_pid PID Controller
 * @brief Generic PID with clamped integral (anti-windup) and output limits.
 * @{
 */

/** @brief PID tuning and limit parameters. */
typedef struct {
    float kp;            /**< Proportional gain. */
    float ki;            /**< Integral gain. */
    float kd;            /**< Derivative gain. */
    float output_min;    /**< Minimum output value. */
    float output_max;    /**< Maximum output value. */
    float integral_max;  /**< Anti-windup integral limit (0 = auto). */
} ecu_pid_config_t;

/** @brief PID controller runtime state. */
typedef struct {
    ecu_pid_config_t cfg;    /**< Active configuration. */
    float integral;          /**< Accumulated integral term. */
    float prev_error;        /**< Previous error for derivative. */
    bool  initialized;       /**< @c true after first update. */
} ecu_pid_t;

/**
 * @brief Initialise a PID controller.
 *
 * @param[out] pid  PID state to initialise.
 * @param[in]  cfg  Tuning parameters.
 */
void ecu_pid_init(ecu_pid_t *pid, const ecu_pid_config_t *cfg);

/**
 * @brief Reset integral and derivative history.
 * @param[in,out] pid  PID state.
 */
void ecu_pid_reset(ecu_pid_t *pid);

/**
 * @brief Compute one PID output.
 *
 * @param[in,out] pid       PID state.
 * @param[in]     setpoint  Desired value.
 * @param[in]     measured  Current measured value.
 * @param[in]     dt        Time step in seconds.
 * @return Clamped PID output.
 */
float ecu_pid_update(ecu_pid_t *pid, float setpoint, float measured, float dt);

/** @} */ /* end ecu_pid */

#endif /* ECU_PID_H */
