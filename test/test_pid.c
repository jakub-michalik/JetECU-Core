#include "test/test_runner.h"
#include "core/ecu_pid.h"

TEST(test_pid_zero_error)
{
    ecu_pid_config_t cfg = { .kp = 1.0f, .ki = 0.0f, .kd = 0.0f,
                              .output_min = -100, .output_max = 100 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float out = ecu_pid_update(&pid, 100.0f, 100.0f, 0.01f);
    ASSERT(out > -0.01f && out < 0.01f);
}

TEST(test_pid_proportional)
{
    ecu_pid_config_t cfg = { .kp = 2.0f, .ki = 0.0f, .kd = 0.0f,
                              .output_min = -100, .output_max = 100 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float out = ecu_pid_update(&pid, 50.0f, 40.0f, 0.01f);
    /* error = 10, kp = 2, output = 20 */
    ASSERT(out > 19.9f && out < 20.1f);
}

TEST(test_pid_output_clamped)
{
    ecu_pid_config_t cfg = { .kp = 100.0f, .ki = 0.0f, .kd = 0.0f,
                              .output_min = 0, .output_max = 50 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float out = ecu_pid_update(&pid, 100.0f, 0.0f, 0.01f);
    ASSERT(out <= 50.0f);
}

TEST(test_pid_step_response)
{
    ecu_pid_config_t cfg = { .kp = 0.5f, .ki = 0.1f, .kd = 0.01f,
                              .output_min = 0, .output_max = 100 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    float value = 0.0f;
    float setpoint = 100.0f;
    float dt = 0.01f;

    /* Run a few steps, value should move toward setpoint */
    for (int i = 0; i < 100; i++) {
        float out = ecu_pid_update(&pid, setpoint, value, dt);
        value += out * dt;  /* simple integration */
    }

    /* Should have moved significantly toward setpoint */
    ASSERT(value > 10.0f);
}

TEST(test_pid_reset)
{
    ecu_pid_config_t cfg = { .kp = 1.0f, .ki = 1.0f, .kd = 0.0f,
                              .output_min = -100, .output_max = 100 };
    ecu_pid_t pid;
    ecu_pid_init(&pid, &cfg);

    /* Accumulate some integral */
    for (int i = 0; i < 10; i++) {
        ecu_pid_update(&pid, 100.0f, 0.0f, 0.01f);
    }

    ecu_pid_reset(&pid);

    /* After reset, first update should not have integral component */
    float out = ecu_pid_update(&pid, 10.0f, 10.0f, 0.01f);
    ASSERT(out > -0.1f && out < 0.1f);
}

int main(void)
{
    printf("=== PID Tests ===\n");
    RUN_TEST(test_pid_zero_error);
    RUN_TEST(test_pid_proportional);
    RUN_TEST(test_pid_output_clamped);
    RUN_TEST(test_pid_step_response);
    RUN_TEST(test_pid_reset);
    TEST_SUMMARY();
}
