/**
 * @file ecu_fuel.h
 * @brief Fuel scheduling with rate limiting and safety cutoff.
 */

#ifndef ECU_FUEL_H
#define ECU_FUEL_H

#include "core/ecu_types.h"
#include "core/ecu_config.h"

/**
 * @defgroup ecu_fuel Fuel Scheduling
 * @brief Rate-limited fuel output with emergency cutoff.
 * @{
 */

/** @brief Fuel command after rate limiting and cutoff logic. */
typedef struct {
    float command_pct;  /**< Requested fuel percentage. */
    float actual_pct;   /**< Output after rate limiting. */
    bool  cutoff;       /**< @c true if safety cutoff is active. */
} ecu_fuel_output_t;

/** @brief Fuel scheduling runtime state. */
typedef struct {
    float last_pct;       /**< Last output percentage. */
    bool  cutoff_active;  /**< Safety cutoff flag. */
} ecu_fuel_state_t;

/**
 * @brief Initialise fuel state.
 * @param[out] state  Fuel state to initialise.
 */
void ecu_fuel_init(ecu_fuel_state_t *state);

/**
 * @brief Compute fuel output with rate limiting.
 *
 * @param[in]     target_pct  Desired fuel percentage (0–100).
 * @param[in]     dt          Time step in seconds.
 * @param[in]     cfg         Engine configuration (ramp rate, limits).
 * @param[in,out] state       Fuel state (updated in place).
 * @return Fuel output with commanded and actual percentages.
 */
ecu_fuel_output_t ecu_fuel_compute(
    float target_pct,
    float dt,
    const ecu_config_t *cfg,
    ecu_fuel_state_t *state
);

/**
 * @brief Activate emergency fuel cutoff.
 * @param[in,out] state  Fuel state.
 */
void ecu_fuel_cutoff(ecu_fuel_state_t *state);

/**
 * @brief Release emergency fuel cutoff.
 * @param[in,out] state  Fuel state.
 */
void ecu_fuel_release(ecu_fuel_state_t *state);

/** @} */ /* end ecu_fuel */

#endif /* ECU_FUEL_H */
