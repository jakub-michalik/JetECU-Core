#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_sensor.h"
}

static ecu_sensor_config_t make_config()
{
    ecu_sensor_config_t cfg = {};
    cfg.range_min = 0.0f;
    cfg.range_max = 1000.0f;
    cfg.rate_limit = 50.0f;
    cfg.stuck_tolerance = 0.5f;
    cfg.stuck_count_limit = 5;
    return cfg;
}

TEST(Sensor, ValidReading) {
    ecu_sensor_config_t cfg = make_config();
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    ecu_sensor_reading_t r = ecu_sensor_validate(500.0f, &cfg, &state);
    EXPECT_TRUE(r.valid);
    EXPECT_EQ(r.faults, SENSOR_FAULT_NONE);
}

TEST(Sensor, RangeLow) {
    ecu_sensor_config_t cfg = make_config();
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    ecu_sensor_reading_t r = ecu_sensor_validate(-10.0f, &cfg, &state);
    EXPECT_FALSE(r.valid);
    EXPECT_TRUE(r.faults & SENSOR_FAULT_RANGE);
}

TEST(Sensor, RangeHigh) {
    ecu_sensor_config_t cfg = make_config();
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    ecu_sensor_reading_t r = ecu_sensor_validate(1500.0f, &cfg, &state);
    EXPECT_FALSE(r.valid);
    EXPECT_TRUE(r.faults & SENSOR_FAULT_RANGE);
}

TEST(Sensor, RateLimit) {
    ecu_sensor_config_t cfg = make_config();
    cfg.stuck_count_limit = 0;
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    ecu_sensor_validate(100.0f, &cfg, &state);

    ecu_sensor_reading_t r = ecu_sensor_validate(200.0f, &cfg, &state);
    EXPECT_TRUE(r.faults & SENSOR_FAULT_RATE);
    EXPECT_LE(r.value, 150.1f);
}

TEST(Sensor, StuckDetection) {
    ecu_sensor_config_t cfg = make_config();
    cfg.rate_limit = 0.0f;
    ecu_sensor_state_t state;
    ecu_sensor_init(&state);

    for (int i = 0; i < 6; i++) {
        ecu_sensor_reading_t r = ecu_sensor_validate(500.0f, &cfg, &state);
        if (i >= 5) {
            EXPECT_TRUE(r.faults & SENSOR_FAULT_STUCK);
            EXPECT_FALSE(r.valid);
        }
    }
}
