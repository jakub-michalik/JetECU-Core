#include "test/test_runner.h"
#include "core/ecu_sensor.h"

static ecu_sensor_config_t make_config(void)
{
    ecu_sensor_config_t cfg = {
        .range_min = 0.0f,
        .range_max = 1000.0f,
        .rate_limit = 50.0f,
        .stuck_tolerance = 0.5f,
        .stuck_count_limit = 5,
    };
    return cfg;
}

TEST(test_sensor_valid_reading)
{
    ecu_sensor_config_t cfg = make_config();
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    ecu_sensor_reading_t r = ecu_sensor_validate(500.0f, &cfg, &state);
    ASSERT(r.valid);
    ASSERT_EQ(r.faults, SENSOR_FAULT_NONE);
}

TEST(test_sensor_range_low)
{
    ecu_sensor_config_t cfg = make_config();
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    ecu_sensor_reading_t r = ecu_sensor_validate(-10.0f, &cfg, &state);
    ASSERT(!r.valid);
    ASSERT(r.faults & SENSOR_FAULT_RANGE);
}

TEST(test_sensor_range_high)
{
    ecu_sensor_config_t cfg = make_config();
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    ecu_sensor_reading_t r = ecu_sensor_validate(1500.0f, &cfg, &state);
    ASSERT(!r.valid);
    ASSERT(r.faults & SENSOR_FAULT_RANGE);
}

TEST(test_sensor_rate_limit)
{
    ecu_sensor_config_t cfg = make_config();
    cfg.stuck_count_limit = 0; /* disable stuck detection for this test */
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    /* First reading establishes baseline */
    ecu_sensor_validate(100.0f, &cfg, &state);

    /* Second reading jumps too fast */
    ecu_sensor_reading_t r = ecu_sensor_validate(200.0f, &cfg, &state);
    ASSERT(r.faults & SENSOR_FAULT_RATE);
    /* Value should be clamped to last + rate_limit */
    ASSERT(r.value <= 150.1f);  /* 100 + 50 */
}

TEST(test_sensor_stuck)
{
    ecu_sensor_config_t cfg = make_config();
    cfg.rate_limit = 0.0f;  /* disable rate limit */
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    /* Send same value repeatedly */
    for (int i = 0; i < 6; i++) {
        ecu_sensor_reading_t r = ecu_sensor_validate(500.0f, &cfg, &state);
        if (i < 5) {
            /* shouldn't trigger yet (first one initializes, then 4 more) */
        } else {
            ASSERT(r.faults & SENSOR_FAULT_STUCK);
            ASSERT(!r.valid);
        }
    }
}

int main(void)
{
    printf("=== Sensor Tests ===\n");
    RUN_TEST(test_sensor_valid_reading);
    RUN_TEST(test_sensor_range_low);
    RUN_TEST(test_sensor_range_high);
    RUN_TEST(test_sensor_rate_limit);
    RUN_TEST(test_sensor_stuck);
    TEST_SUMMARY();
}
