#include "test/test_runner.h"
#include "core/ecu_fuel.h"

static ecu_config_t test_cfg(void)
{
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    return cfg;
}

TEST(test_fuel_init)
{
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();
    ecu_fuel_output_t out = ecu_fuel_compute(0.0f, 0.01f, &cfg, &state);
    ASSERT(out.actual_pct < 0.01f);
    ASSERT(!out.cutoff);
}

TEST(test_fuel_rate_limit)
{
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();
    cfg.fuel_ramp_rate = 10.0f;  /* 10%/sec */

    /* Try to jump to 100% in 0.1s — should be limited to 1% */
    ecu_fuel_output_t out = ecu_fuel_compute(100.0f, 0.1f, &cfg, &state);
    ASSERT(out.actual_pct < 2.0f);  /* should be ~1% */
}

TEST(test_fuel_cutoff)
{
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();

    /* Get to some fuel level */
    for (int i = 0; i < 100; i++) {
        ecu_fuel_compute(50.0f, 0.1f, &cfg, &state);
    }

    /* Cutoff */
    ecu_fuel_cutoff(&state);
    ecu_fuel_output_t out = ecu_fuel_compute(50.0f, 0.1f, &cfg, &state);
    ASSERT(out.actual_pct < 0.01f);
    ASSERT(out.cutoff);
}

TEST(test_fuel_cutoff_release)
{
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);
    ecu_config_t cfg = test_cfg();

    ecu_fuel_cutoff(&state);
    ecu_fuel_release(&state);

    ecu_fuel_output_t out = ecu_fuel_compute(50.0f, 1.0f, &cfg, &state);
    ASSERT(!out.cutoff);
    ASSERT(out.actual_pct > 0.0f);
}

TEST(test_fuel_max_clamp)
{
    ecu_fuel_state_t state;
    ecu_fuel_init(&state);

    ecu_config_t cfg = test_cfg();
    cfg.fuel_max_pct = 80.0f;
    cfg.fuel_ramp_rate = 10000.0f;  /* no rate limit effectively */

    ecu_fuel_output_t out = ecu_fuel_compute(100.0f, 1.0f, &cfg, &state);
    ASSERT(out.actual_pct <= 80.1f);
}

int main(void)
{
    printf("=== Fuel Tests ===\n");
    RUN_TEST(test_fuel_init);
    RUN_TEST(test_fuel_rate_limit);
    RUN_TEST(test_fuel_cutoff);
    RUN_TEST(test_fuel_cutoff_release);
    RUN_TEST(test_fuel_max_clamp);
    TEST_SUMMARY();
}
