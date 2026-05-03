/**
 * @file ecu_dsp.c
 * @brief Implementation of the DSP primitives declared in ecu_dsp.h.
 *
 * Biquad section uses Direct Form II Transposed:
 *
 *     y[n] = b0*x[n] + s1
 *     s1   = b1*x[n] - a1*y[n] + s2
 *     s2   = b2*x[n] - a2*y[n]
 *
 * Coefficients are pre-normalised so that a0 = 1.
 */

#include "core/ecu_dsp.h"

#include <math.h>

#ifndef ECU_PI_F
#define ECU_PI_F 3.14159265358979323846f
#endif

void ecu_biquad_init(ecu_biquad_t *f, const ecu_biquad_coef_t *coef)
{
    if (f == NULL || coef == NULL) {
        return;
    }
    f->c  = *coef;
    f->s1 = 0.0f;
    f->s2 = 0.0f;
}

void ecu_biquad_reset(ecu_biquad_t *f)
{
    if (f == NULL) {
        return;
    }
    f->s1 = 0.0f;
    f->s2 = 0.0f;
}

float ecu_biquad_step(ecu_biquad_t *f, float x)
{
    const float y = f->c.b0 * x + f->s1;
    f->s1 = f->c.b1 * x - f->c.a1 * y + f->s2;
    f->s2 = f->c.b2 * x - f->c.a2 * y;
    return y;
}

ecu_err_t ecu_dsp_design_lowpass(ecu_biquad_coef_t *coef,
                                 float fc_hz, float fs_hz)
{
    if (coef == NULL || !(fs_hz > 0.0f) ||
        !(fc_hz > 0.0f) || !(fc_hz < 0.5f * fs_hz)) {
        return ECU_ERR_INVALID_PARAM;
    }

    /* Pre-warped analog cutoff: omega = tan(pi * fc / fs). */
    const float omega = tanf(ECU_PI_F * fc_hz / fs_hz);
    const float omega2 = omega * omega;
    /* Butterworth Q = 1/sqrt(2). */
    const float sqrt2 = 1.41421356237f;
    const float norm  = 1.0f / (1.0f + sqrt2 * omega + omega2);

    coef->b0 = omega2 * norm;
    coef->b1 = 2.0f * coef->b0;
    coef->b2 = coef->b0;
    coef->a1 = 2.0f * (omega2 - 1.0f) * norm;
    coef->a2 = (1.0f - sqrt2 * omega + omega2) * norm;
    return ECU_OK;
}

ecu_err_t ecu_dsp_design_notch(ecu_biquad_coef_t *coef,
                               float f0_hz, float bw_hz, float fs_hz)
{
    if (coef == NULL || !(fs_hz > 0.0f) ||
        !(f0_hz > 0.0f) || !(f0_hz < 0.5f * fs_hz) ||
        !(bw_hz > 0.0f) || !(bw_hz < fs_hz)) {
        return ECU_ERR_INVALID_PARAM;
    }

    /* RBJ Audio EQ Cookbook — biquad notch.
     * w0 = 2*pi*f0/fs ; alpha = sin(w0)*sinh(ln(2)/2 * BW * w0/sin(w0))
     * Simplified using alpha = tan(pi*BW/fs).
     */
    const float w0    = 2.0f * ECU_PI_F * f0_hz / fs_hz;
    const float cw0   = cosf(w0);
    const float alpha = tanf(ECU_PI_F * bw_hz / fs_hz);
    const float a0    = 1.0f + alpha;
    const float norm  = 1.0f / a0;

    coef->b0 =  1.0f * norm;
    coef->b1 = -2.0f * cw0 * norm;
    coef->b2 =  1.0f * norm;
    coef->a1 = -2.0f * cw0 * norm;
    coef->a2 = (1.0f - alpha) * norm;
    return ECU_OK;
}

void ecu_median3_init(ecu_median3_t *m)
{
    if (m == NULL) {
        return;
    }
    m->x0 = 0.0f;
    m->x1 = 0.0f;
    m->filled = 0;
}

float ecu_median3_step(ecu_median3_t *m, float x)
{
    if (m->filled < 2) {
        /* Not enough history yet — pass through. */
        m->x0 = m->x1;
        m->x1 = x;
        m->filled++;
        return x;
    }

    const float a = m->x0;
    const float b = m->x1;
    const float c = x;
    /* Median of three via two comparisons + a selection. */
    float med;
    if (a <= b) {
        if (b <= c)      med = b;       /* a <= b <= c */
        else if (a <= c) med = c;       /* a <= c < b  */
        else             med = a;       /* c < a <= b  */
    } else {
        if (a <= c)      med = a;       /* b < a <= c  */
        else if (b <= c) med = c;       /* b <= c < a  */
        else             med = b;       /* c < b < a   */
    }

    m->x0 = m->x1;
    m->x1 = x;
    return med;
}
