#include "test/test_runner.h"
#include "core/ecu_core.h"
#include <stdlib.h>
#include <time.h>

/* Simple PRNG for deterministic fuzz */
static uint32_t fuzz_seed = 42;
static uint32_t fuzz_rand(void)
{
    fuzz_seed = fuzz_seed * 1664525 + 1013904223;
    return fuzz_seed;
}

static float fuzz_float(float lo, float hi)
{
    return lo + (hi - lo) * ((float)(fuzz_rand() % 10000) / 10000.0f);
}

TEST(test_fuzz_random_inputs)
{
    ecu_t ecu;
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    /* Run 10000 steps with random inputs — should not crash */
    for (int i = 0; i < 10000; i++) {
        ecu_inputs_t in = {
            .rpm = fuzz_float(-1000, 200000),
            .egt = fuzz_float(-100, 2000),
            .throttle = fuzz_float(-10, 110),
        };
        ecu_step(&ecu, &in, 0.001f);
    }
    ASSERT(1);  /* if we get here, no crash */
}

TEST(test_fuzz_sensor_noise)
{
    ecu_t ecu;
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    float base_rpm = 0;
    float base_egt = 25;
    float throttle = 50;

    for (int i = 0; i < 5000; i++) {
        float noise_rpm = fuzz_float(-500, 500);
        float noise_egt = fuzz_float(-20, 20);

        ecu_inputs_t in = {
            .rpm = base_rpm + noise_rpm,
            .egt = base_egt + noise_egt,
            .throttle = throttle,
        };

        ecu_outputs_t out = ecu_step(&ecu, &in, 0.001f);

        /* Fuel should always be in valid range */
        ASSERT(out.fuel_pct >= 0.0f);
        ASSERT(out.fuel_pct <= 110.0f);

        base_rpm += 10;  /* slowly ramp */
        if (base_rpm > 50000) base_egt += 0.5f;
    }
    ASSERT(1);
}

TEST(test_fuzz_config_values)
{
    /* Test with extreme config values */
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);

    /* Very low limits */
    cfg.rpm_max = 1000;
    cfg.egt_max = 100;

    ecu_t ecu;
    ecu_init(&ecu, &cfg);

    ecu_inputs_t in = { .rpm = 500, .egt = 50, .throttle = 50 };
    for (int i = 0; i < 1000; i++) {
        ecu_step(&ecu, &in, 0.001f);
    }
    ASSERT(1);
}

int main(void)
{
    printf("=== Fuzz Tests ===\n");
    RUN_TEST(test_fuzz_random_inputs);
    RUN_TEST(test_fuzz_sensor_noise);
    RUN_TEST(test_fuzz_config_values);
    TEST_SUMMARY();
}
