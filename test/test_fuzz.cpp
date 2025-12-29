#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_core.h"
}

static uint32_t fuzz_seed = 42;

static uint32_t fuzz_rand()
{
    fuzz_seed = fuzz_seed * 1664525 + 1013904223;
    return fuzz_seed;
}

static float fuzz_float(float lo, float hi)
{
    return lo + (hi - lo) * (static_cast<float>(fuzz_rand() % 10000) / 10000.0f);
}

TEST(Fuzz, RandomInputs) {
    ecu_t ecu;
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    for (int i = 0; i < 10000; i++) {
        ecu_inputs_t in = {
            fuzz_float(-1000, 200000),
            fuzz_float(-100, 2000),
            fuzz_float(-10, 110),
        };
        ecu_step(&ecu, &in, 0.001f);
    }
}

TEST(Fuzz, SensorNoise) {
    ecu_t ecu;
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    float base_rpm = 0;
    float base_egt = 25;

    for (int i = 0; i < 5000; i++) {
        ecu_inputs_t in = {
            base_rpm + fuzz_float(-500, 500),
            base_egt + fuzz_float(-20, 20),
            50.0f,
        };

        ecu_outputs_t out = ecu_step(&ecu, &in, 0.001f);
        EXPECT_GE(out.fuel_pct, 0.0f);
        EXPECT_LE(out.fuel_pct, 110.0f);

        base_rpm += 10;
        if (base_rpm > 50000) base_egt += 0.5f;
    }
}

TEST(Fuzz, ExtremeConfig) {
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    cfg.rpm_max = 1000;
    cfg.egt_max = 100;

    ecu_t ecu;
    ecu_init(&ecu, &cfg);

    ecu_inputs_t in = { 500, 50, 50 };
    for (int i = 0; i < 1000; i++) {
        ecu_step(&ecu, &in, 0.001f);
    }
}
