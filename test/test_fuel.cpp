#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_fuel.h"
}

static ecu_config_t test_cfg()
{
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    return cfg;
}

TEST(Fuel, Init) {
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();
    ecu_fuel_output_t out = ecu_fuel_compute(0.0f, 0.01f, &cfg, &state);
    EXPECT_LT(out.actual_pct, 0.01f);
    EXPECT_FALSE(out.cutoff);
}

TEST(Fuel, RateLimit) {
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();
    cfg.fuel_ramp_rate = 10.0f;

    ecu_fuel_output_t out = ecu_fuel_compute(100.0f, 0.1f, &cfg, &state);
    EXPECT_LT(out.actual_pct, 2.0f);
}

TEST(Fuel, Cutoff) {
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();

    for (int i = 0; i < 100; i++) {
        ecu_fuel_compute(50.0f, 0.1f, &cfg, &state);
    }

    ecu_fuel_cutoff(&state);
    ecu_fuel_output_t out = ecu_fuel_compute(50.0f, 0.1f, &cfg, &state);
    EXPECT_LT(out.actual_pct, 0.01f);
    EXPECT_TRUE(out.cutoff);
}

TEST(Fuel, CutoffRelease) {
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);
    ecu_config_t cfg = test_cfg();

    ecu_fuel_cutoff(&state);
    ecu_fuel_release(&state);

    ecu_fuel_output_t out = ecu_fuel_compute(50.0f, 1.0f, &cfg, &state);
    EXPECT_FALSE(out.cutoff);
    EXPECT_GT(out.actual_pct, 0.0f);
}

TEST(Fuel, MaxClamp) {
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();
    cfg.fuel_max_pct = 80.0f;
    cfg.fuel_ramp_rate = 10000.0f;

    ecu_fuel_output_t out = ecu_fuel_compute(100.0f, 1.0f, &cfg, &state);
    EXPECT_LE(out.actual_pct, 80.1f);
}
