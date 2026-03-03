/**
 * @file ecu_config.h
 * @brief Engine configuration parameters.
 */

#ifndef ECU_CONFIG_H
#define ECU_CONFIG_H

#include "core/ecu_types.h"

/**
 * @defgroup ecu_config Configuration
 * @brief Data-driven engine tuning parameters loaded from JSON.
 * @{
 */

/** @brief Complete engine configuration. */
typedef struct {
    /* RPM thresholds */
    float rpm_idle;          /**< Idle RPM. */
    float rpm_max;           /**< Maximum allowable RPM. */
    float rpm_start_target;  /**< Target RPM for startup sequence. */

    /* EGT limits (degC) */
    float egt_max;           /**< Absolute EGT limit (deg C). */
    float egt_start_min;     /**< Minimum EGT confirming ignition (deg C). */

    /* Fuel */
    float fuel_start_pct;    /**< Fuel percentage during ignition phase. */
    float fuel_max_pct;      /**< Maximum fuel output (%). */
    float fuel_ramp_rate;    /**< Fuel ramp rate (% per second). */

    /* Timeouts (ms) */
    uint32_t prestart_timeout_ms;  /**< Prestart phase timeout. */
    uint32_t spinup_timeout_ms;    /**< Spinup phase timeout. */
    uint32_t ignition_timeout_ms;  /**< Ignition phase timeout. */

    /* RPM ramp */
    float rpm_ramp_rate;     /**< RPM ramp rate during acceleration. */

    /* PID tuning */
    float pid_kp;            /**< PID proportional gain. */
    float pid_ki;            /**< PID integral gain. */
    float pid_kd;            /**< PID derivative gain. */
} ecu_config_t;

/**
 * @brief Load default configuration values.
 * @param[out] cfg  Configuration struct to populate.
 */
void ecu_config_defaults(ecu_config_t *cfg);

/** @} */ /* end ecu_config */

#endif /* ECU_CONFIG_H */
