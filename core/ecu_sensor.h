/**
 * @file ecu_sensor.h
 * @brief Multi-stage sensor validation and filtering.
 */

#ifndef ECU_SENSOR_H
#define ECU_SENSOR_H

#include "core/ecu_types.h"

/**
 * @defgroup ecu_sensor Sensor Processing
 * @brief Range, rate, stuck-at, and plausibility checks for sensor readings.
 * @{
 */

/** @name Sensor fault flags
 * @{
 */
#define SENSOR_FAULT_NONE       0x00  /**< No fault detected. */
#define SENSOR_FAULT_RANGE      0x01  /**< Value outside valid range. */
#define SENSOR_FAULT_RATE       0x02  /**< Rate-of-change limit exceeded. */
#define SENSOR_FAULT_STUCK      0x04  /**< Value stuck (unchanging). */
#define SENSOR_FAULT_PLAUSIBLE  0x08  /**< Cross-check plausibility failure. */
/** @} */

/** @brief Processed sensor reading with validation result. */
typedef struct {
    float    value;   /**< Processed (validated) value. */
    float    raw;     /**< Original raw reading. */
    bool     valid;   /**< @c true if all checks pass. */
    uint32_t faults;  /**< Bitmask of @c SENSOR_FAULT_* flags. */
} ecu_sensor_reading_t;

/** @brief Per-sensor validation configuration. */
typedef struct {
    float range_min;        /**< Minimum valid value. */
    float range_max;        /**< Maximum valid value. */
    float rate_limit;       /**< Maximum change per step. */
    float stuck_tolerance;  /**< Threshold for stuck detection. */
    int   stuck_count_limit;/**< Consecutive stuck counts to trigger fault. */
} ecu_sensor_config_t;

/** @brief Per-sensor runtime state (history for stuck detection). */
typedef struct {
    float last_value;   /**< Previous validated value. */
    int   stuck_count;  /**< Consecutive stuck readings. */
    bool  initialized;  /**< @c true after first reading processed. */
} ecu_sensor_state_t;

/**
 * @brief Initialise sensor state.
 * @param[out] state  Sensor state to initialise.
 */
void ecu_sensor_init(ecu_sensor_state_t *state);

/**
 * @brief Validate a raw sensor reading.
 *
 * Applies range, rate-of-change, and stuck-at checks.
 *
 * @param[in]     raw    Raw sensor value.
 * @param[in]     cfg    Validation configuration.
 * @param[in,out] state  Sensor state (updated in place).
 * @return Processed reading with validity and fault flags.
 */
ecu_sensor_reading_t ecu_sensor_validate(
    float raw,
    const ecu_sensor_config_t *cfg,
    ecu_sensor_state_t *state
);

/**
 * @brief Cross-check EGT plausibility against RPM.
 *
 * @param[in] egt       Exhaust gas temperature (deg C).
 * @param[in] rpm       Current shaft RPM.
 * @param[in] rpm_idle  Idle RPM threshold.
 * @return @c true if the EGT is plausible for the given RPM.
 */
bool ecu_sensor_egt_plausible(float egt, float rpm, float rpm_idle);

/** @} */ /* end ecu_sensor */

#endif /* ECU_SENSOR_H */
