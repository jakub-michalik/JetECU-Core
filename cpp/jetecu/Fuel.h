#pragma once

#ifndef JETECU_FUEL_H
#define JETECU_FUEL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_fuel.h"
#ifdef __cplusplus
}
#endif

namespace jetecu {

/** @brief Result of a fuel computation step. */
struct FuelOutput {
    float command_pct;  /**< Requested fuel percentage. */
    float actual_pct;   /**< Output after rate limiting. */
    bool  cutoff;       /**< True if safety cutoff is active. */
};

/**
 * @brief C++ wrapper around ecu_fuel_state_t.
 *
 * Provides rate-limited fuel scheduling with emergency cutoff,
 * including an RAII CutoffGuard for exception-safe scoping.
 */
class Fuel {
public:
    Fuel();

    /**
     * @brief Compute fuel output with rate limiting.
     * @param target_pct Desired fuel percentage (0-100).
     * @param dt         Time step in seconds.
     * @param cfg        Engine configuration.
     * @return FuelOutput with commanded/actual percentages and cutoff status.
     */
    FuelOutput compute(float target_pct, float dt, const ecu_config_t &cfg);

    /** @brief Activate emergency fuel cutoff. */
    void cutoff();

    /** @brief Release emergency fuel cutoff. */
    void release();

    /** @brief Check if cutoff is currently active. */
    bool isCutoff() const;

    /**
     * @brief RAII guard that activates cutoff on construction
     *        and releases on destruction.
     *
     * Useful for exception-safe fuel cutoff scoping:
     * @code
     *   {
     *       Fuel::CutoffGuard guard(fuel);
     *       // fuel is cut off here
     *   } // automatically released
     * @endcode
     */
    class CutoffGuard {
    public:
        explicit CutoffGuard(Fuel &fuel);
        ~CutoffGuard();

        CutoffGuard(const CutoffGuard &) = delete;
        CutoffGuard &operator=(const CutoffGuard &) = delete;

    private:
        Fuel &fuel_;
    };

    ecu_fuel_state_t &raw() { return state_; }
    const ecu_fuel_state_t &raw() const { return state_; }

private:
    ecu_fuel_state_t state_;
};

} // namespace jetecu

#endif // JETECU_FUEL_H
