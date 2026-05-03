/**
 * @file ecu_dsp.h
 * @brief Digital signal processing primitives for sensor conditioning.
 *
 * The DSP module provides building blocks that run before
 * :ref:`sensor validation <ecu_sensor>`:
 *
 *  - 2nd-order IIR biquad in Direct Form II Transposed (numerically
 *    stable when implemented in single-precision float on a 32-bit MCU).
 *  - Coefficient designers for Butterworth low-pass and 2nd-order notch
 *    filters via the bilinear transform with pre-warping.
 *  - 3-sample median filter for impulsive spike rejection (cheap,
 *    O(1) memory, ideal for thermocouple readings).
 *
 * All routines are allocation-free and re-entrant; each filter owns its
 * own state.  No transcendental functions are called on the hot path —
 * `sinf`/`cosf`/`tanf` are only used inside the *design* functions which
 * are typically called once at boot.
 *
 * @{
 */

#ifndef ECU_DSP_H
#define ECU_DSP_H

#include "core/ecu_types.h"

/**
 * @defgroup ecu_dsp DSP
 * @brief Sensor pre-conditioning: biquad IIR filters and spike rejection.
 * @{
 */

/**
 * @brief Coefficients for a 2nd-order IIR section.
 *
 * Difference equation (a0 is normalised to 1):
 *
 *   y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
 *                  - a1*y[n-1] - a2*y[n-2]
 */
typedef struct {
    float b0; /**< Numerator coefficient for x[n]. */
    float b1; /**< Numerator coefficient for x[n-1]. */
    float b2; /**< Numerator coefficient for x[n-2]. */
    float a1; /**< Denominator coefficient for y[n-1]. */
    float a2; /**< Denominator coefficient for y[n-2]. */
} ecu_biquad_coef_t;

/**
 * @brief Runtime state for a single biquad section (Direct Form II
 *        Transposed).
 *
 * DF2T trades one extra adder for better numerical behaviour when the
 * filter is implemented with finite-precision arithmetic; the two state
 * variables @c s1 and @c s2 store the transposed delay line.
 */
typedef struct {
    ecu_biquad_coef_t c;  /**< Filter coefficients. */
    float s1;             /**< Internal state, post-b0 stage. */
    float s2;             /**< Internal state, post-b1/a1 stage. */
} ecu_biquad_t;

/**
 * @brief Initialise (or reset) a biquad with explicit coefficients.
 *
 * @param[out] f     Filter state.
 * @param[in]  coef  Coefficients to load.
 */
void ecu_biquad_init(ecu_biquad_t *f, const ecu_biquad_coef_t *coef);

/**
 * @brief Clear the delay line without changing coefficients.
 *
 * Useful after a discontinuity (e.g. sensor reconnect) to suppress the
 * transient that would otherwise ring through the filter.
 *
 * @param[in,out] f  Filter state.
 */
void ecu_biquad_reset(ecu_biquad_t *f);

/**
 * @brief Process one input sample.
 *
 * Hot path — no branches, no transcendentals; 5 multiplies + 4 adds.
 *
 * @param[in,out] f  Filter state.
 * @param[in]     x  Input sample.
 * @return Filtered output sample.
 */
float ecu_biquad_step(ecu_biquad_t *f, float x);

/**
 * @brief Design a 2nd-order Butterworth low-pass filter.
 *
 * Pre-warps the analog prototype with the bilinear transform so the
 * digital cutoff matches @p fc_hz exactly.  Result is normalised so
 * that DC gain is 1.0.
 *
 * @param[out] coef    Coefficient destination.
 * @param[in]  fc_hz   Cutoff frequency in Hz (0 < fc < fs/2).
 * @param[in]  fs_hz   Sample rate in Hz.
 * @return ECU_OK on success, ECU_ERR_INVALID_PARAM if fc is out of range.
 */
ecu_err_t ecu_dsp_design_lowpass(ecu_biquad_coef_t *coef,
                                 float fc_hz, float fs_hz);

/**
 * @brief Design a 2nd-order notch filter.
 *
 * Rejects a narrow band centred at @p f0_hz with -3 dB half-width
 * @p bw_hz.  Useful for cancelling a known mechanical harmonic
 * (e.g. shaft resonance, propeller blade pass).
 *
 * @param[out] coef    Coefficient destination.
 * @param[in]  f0_hz   Notch centre frequency in Hz.
 * @param[in]  bw_hz   Notch bandwidth in Hz.
 * @param[in]  fs_hz   Sample rate in Hz.
 * @return ECU_OK on success, ECU_ERR_INVALID_PARAM if parameters invalid.
 */
ecu_err_t ecu_dsp_design_notch(ecu_biquad_coef_t *coef,
                               float f0_hz, float bw_hz, float fs_hz);

/**
 * @brief State for a 3-sample running median filter.
 *
 * Rejects single-sample impulses (ADC glitches, EMC transients) while
 * preserving step responses far better than a moving average.
 */
typedef struct {
    float x0;       /**< Two-samples-ago value. */
    float x1;       /**< Previous value. */
    uint8_t filled; /**< Number of valid samples in the window (0..2). */
} ecu_median3_t;

/**
 * @brief Initialise / reset a median-of-3 filter.
 * @param[out] m  Filter state.
 */
void ecu_median3_init(ecu_median3_t *m);

/**
 * @brief Push one sample through the 3-sample median filter.
 *
 * Until the filter has seen at least three samples, the most recent
 * sample is passed through unchanged.
 *
 * @param[in,out] m  Filter state.
 * @param[in]     x  New sample.
 * @return Median of the three most recent samples.
 */
float ecu_median3_step(ecu_median3_t *m, float x);

/** @} */ /* end ecu_dsp */

#endif /* ECU_DSP_H */

/** @} */
