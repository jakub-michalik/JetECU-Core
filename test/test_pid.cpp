#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_pid.h"
}

TEST(PID, ZeroError) {
    ecu_pid_config_t cfg = { 1.0f, 0.0f, 0.0f, -100, 100, 0 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float out = ecu_pid_update(&pid, 100.0f, 100.0f, 0.01f);
    EXPECT_NEAR(out, 0.0f, 0.01f);
}

TEST(PID, Proportional) {
    ecu_pid_config_t cfg = { 2.0f, 0.0f, 0.0f, -100, 100, 0 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float out = ecu_pid_update(&pid, 50.0f, 40.0f, 0.01f);
    EXPECT_NEAR(out, 20.0f, 0.1f);
}

TEST(PID, OutputClamped) {
    ecu_pid_config_t cfg = { 100.0f, 0.0f, 0.0f, 0, 50, 0 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float out = ecu_pid_update(&pid, 100.0f, 0.0f, 0.01f);
    EXPECT_LE(out, 50.0f);
}

TEST(PID, StepResponse) {
    ecu_pid_config_t cfg = { 0.5f, 0.1f, 0.01f, 0, 100, 0 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float value = 0.0f;
    float dt = 0.01f;

    for (int i = 0; i < 100; i++) {
        float out = ecu_pid_update(&pid, 100.0f, value, dt);
        value += out * dt;
    }

    EXPECT_GT(value, 10.0f);
}

TEST(PID, Reset) {
    ecu_pid_config_t cfg = { 1.0f, 1.0f, 0.0f, -100, 100, 0 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    for (int i = 0; i < 10; i++) {
        ecu_pid_update(&pid, 100.0f, 0.0f, 0.01f);
    }

    ecu_pid_reset(&pid);
    float out = ecu_pid_update(&pid, 10.0f, 10.0f, 0.01f);
    EXPECT_NEAR(out, 0.0f, 0.1f);
}
