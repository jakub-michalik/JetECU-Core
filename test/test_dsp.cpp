/**
 * Tests for the DSP primitives in core/ecu_dsp.h.
 *
 * Covers:
 *   - Biquad correctness (DC gain, step convergence, attenuation at fs/2).
 *   - Butterworth low-pass attenuation at 4x cutoff (should be ~-24 dB).
 *   - Notch filter rejects its centre frequency, passes a far-away tone.
 *   - Median-of-3 rejects single-sample spikes, preserves steps.
 *   - Invalid parameter handling.
 */

#include <gtest/gtest.h>
#include <cmath>

extern "C" {
#include "core/ecu_dsp.h"
}

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kFs = 1000.0f;   /* 1 kHz sample rate — matches ECU loop. */

float run_sine(ecu_biquad_t* f, float freq_hz, float fs, int settle, int meas)
{
    /* Drive the filter with a unit sine and return peak-to-peak / 2 (amplitude)
     * once it has settled. */
    const float omega = 2.0f * kPi * freq_hz / fs;
    float ymax = -1e30f, ymin = 1e30f;
    for (int n = 0; n < settle + meas; ++n) {
        const float y = ecu_biquad_step(f, sinf(omega * n));
        if (n >= settle) {
            if (y > ymax) ymax = y;
            if (y < ymin) ymin = y;
        }
    }
    return 0.5f * (ymax - ymin);
}

}  // namespace

TEST(BiquadLowpass, DesignReturnsOkForValidParams)
{
    ecu_biquad_coef_t c{};
    EXPECT_EQ(ecu_dsp_design_lowpass(&c, 50.0f, kFs), ECU_OK);
}

TEST(BiquadLowpass, DesignRejectsOutOfRange)
{
    ecu_biquad_coef_t c{};
    EXPECT_EQ(ecu_dsp_design_lowpass(&c, 0.0f, kFs),        ECU_ERR_INVALID_PARAM);
    EXPECT_EQ(ecu_dsp_design_lowpass(&c, kFs * 0.5f, kFs),  ECU_ERR_INVALID_PARAM);
    EXPECT_EQ(ecu_dsp_design_lowpass(&c, -1.0f, kFs),       ECU_ERR_INVALID_PARAM);
    EXPECT_EQ(ecu_dsp_design_lowpass(nullptr, 50.0f, kFs),  ECU_ERR_INVALID_PARAM);
}

TEST(BiquadLowpass, DcGainIsUnity)
{
    ecu_biquad_coef_t c{};
    ASSERT_EQ(ecu_dsp_design_lowpass(&c, 50.0f, kFs), ECU_OK);
    ecu_biquad_t f{};
    ecu_biquad_init(&f, &c);

    /* Feed 2000 samples of DC=1; output must converge to 1. */
    float y = 0.0f;
    for (int i = 0; i < 2000; ++i) y = ecu_biquad_step(&f, 1.0f);
    EXPECT_NEAR(y, 1.0f, 1e-4f);
}

TEST(BiquadLowpass, AttenuatesAboveCutoff)
{
    ecu_biquad_coef_t c{};
    ASSERT_EQ(ecu_dsp_design_lowpass(&c, 50.0f, kFs), ECU_OK);

    /* Pass-band: 10 Hz should come through near unity. */
    ecu_biquad_t pass{};
    ecu_biquad_init(&pass, &c);
    const float amp_pass = run_sine(&pass, 10.0f, kFs, 500, 1000);
    EXPECT_GT(amp_pass, 0.95f);
    EXPECT_LT(amp_pass, 1.05f);

    /* Stop-band: 200 Hz (4x cutoff). 2nd-order Butterworth ~-24 dB. */
    ecu_biquad_t stop{};
    ecu_biquad_init(&stop, &c);
    const float amp_stop = run_sine(&stop, 200.0f, kFs, 500, 1000);
    EXPECT_LT(amp_stop, 0.10f);  /* < -20 dB */
}

TEST(BiquadLowpass, ResetClearsState)
{
    ecu_biquad_coef_t c{};
    ASSERT_EQ(ecu_dsp_design_lowpass(&c, 50.0f, kFs), ECU_OK);
    ecu_biquad_t f{};
    ecu_biquad_init(&f, &c);

    for (int i = 0; i < 100; ++i) ecu_biquad_step(&f, 5.0f);
    ecu_biquad_reset(&f);
    EXPECT_NEAR(ecu_biquad_step(&f, 0.0f), 0.0f, 1e-6f);
}

TEST(BiquadNotch, RejectsCentreFrequency)
{
    ecu_biquad_coef_t c{};
    ASSERT_EQ(ecu_dsp_design_notch(&c, 50.0f, 4.0f, kFs), ECU_OK);

    ecu_biquad_t notch{};
    ecu_biquad_init(&notch, &c);
    const float amp_at_notch = run_sine(&notch, 50.0f, kFs, 1000, 2000);
    EXPECT_LT(amp_at_notch, 0.05f);  /* > -26 dB rejection */

    ecu_biquad_t pass{};
    ecu_biquad_init(&pass, &c);
    const float amp_far = run_sine(&pass, 5.0f, kFs, 500, 1000);
    EXPECT_GT(amp_far, 0.90f);  /* far from notch is preserved */
}

TEST(Median3, PassesThroughWhilePriming)
{
    ecu_median3_t m{};
    ecu_median3_init(&m);
    EXPECT_FLOAT_EQ(ecu_median3_step(&m, 1.0f), 1.0f);
    EXPECT_FLOAT_EQ(ecu_median3_step(&m, 2.0f), 2.0f);
}

TEST(Median3, RejectsSpike)
{
    ecu_median3_t m{};
    ecu_median3_init(&m);
    /* prime */
    ecu_median3_step(&m, 1.0f);
    ecu_median3_step(&m, 1.0f);
    /* spike */
    EXPECT_FLOAT_EQ(ecu_median3_step(&m, 99.0f), 1.0f);
    /* recovery */
    EXPECT_FLOAT_EQ(ecu_median3_step(&m, 1.0f), 1.0f);
}

TEST(Median3, PreservesStep)
{
    ecu_median3_t m{};
    ecu_median3_init(&m);
    ecu_median3_step(&m, 0.0f);
    ecu_median3_step(&m, 0.0f);
    /* Step to 10: median(0, 0, 10) = 0 — one sample of group delay. */
    EXPECT_FLOAT_EQ(ecu_median3_step(&m, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(ecu_median3_step(&m, 10.0f), 10.0f);
    EXPECT_FLOAT_EQ(ecu_median3_step(&m, 10.0f), 10.0f);
}
