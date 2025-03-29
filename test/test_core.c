#include "test/test_runner.h"
#include "core/ecu_core.h"

static ecu_t make_ecu(void)
{
    ecu_t ecu;
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);
    return ecu;
}

TEST(test_core_init)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = { .rpm = 0, .egt = 25.0f, .throttle = 0 };
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.01f);
    ASSERT_EQ(out.state, ECU_STATE_OFF);
    ASSERT(out.fuel_pct < 0.01f);
}

TEST(test_core_start_sequence)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = { .rpm = 0, .egt = 25.0f, .throttle = 50.0f };

    ecu_outputs_t out = ecu_step(&ecu, &in, 0.01f);
    ASSERT_EQ(out.state, ECU_STATE_SPINUP);

    in.rpm = 5000.0f;
    out = ecu_step(&ecu, &in, 0.1f);
    ASSERT(out.starter_on);

    in.rpm = 15000.0f;
    out = ecu_step(&ecu, &in, 0.1f);
    ASSERT_EQ(out.state, ECU_STATE_IGNITION);
}

TEST(test_core_ignition_to_ramp)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = { .rpm = 0, .egt = 25.0f, .throttle = 50.0f };

    ecu_step(&ecu, &in, 0.01f);
    in.rpm = 15000.0f;
    ecu_step(&ecu, &in, 0.1f);

    in.egt = 200.0f;
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.1f);
    ASSERT_EQ(out.state, ECU_STATE_RAMP);
}

TEST(test_core_fault_on_timeout)
{
    ecu_t ecu = make_ecu();
    ecu.config.spinup_timeout_ms = 100;

    ecu_inputs_t in = { .rpm = 0, .egt = 25.0f, .throttle = 50.0f };
    ecu_step(&ecu, &in, 0.01f);

    in.rpm = 100.0f;
    for (int i = 0; i < 20; i++) {
        ecu_step(&ecu, &in, 0.01f);
    }
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.01f);
    ASSERT_EQ(out.state, ECU_STATE_FAULT);
}

TEST(test_core_egt_overtemp)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = { .rpm = 0, .egt = 25.0f, .throttle = 50.0f };

    ecu_step(&ecu, &in, 0.01f);
    in.rpm = 15000.0f;
    ecu_step(&ecu, &in, 0.1f);
    in.egt = 200.0f;
    ecu_step(&ecu, &in, 0.1f);

    in.egt = 1200.0f;
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.1f);
    ASSERT_EQ(out.state, ECU_STATE_FAULT);
    ASSERT(out.fault_code != 0);
}

TEST(test_core_overspeed)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = { .rpm = 0, .egt = 25.0f, .throttle = 50.0f };

    ecu_step(&ecu, &in, 0.01f);
    in.rpm = 15000.0f;
    ecu_step(&ecu, &in, 0.1f);
    in.egt = 200.0f;
    ecu_step(&ecu, &in, 0.1f);
    in.rpm = 58000.0f;
    ecu_step(&ecu, &in, 0.1f);

    in.rpm = 130000.0f;
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.1f);
    ASSERT_EQ(out.state, ECU_STATE_FAULT);
}

int main(void)
{
    printf("=== Core Tests ===\n");
    RUN_TEST(test_core_init);
    RUN_TEST(test_core_start_sequence);
    RUN_TEST(test_core_ignition_to_ramp);
    RUN_TEST(test_core_fault_on_timeout);
    RUN_TEST(test_core_egt_overtemp);
    RUN_TEST(test_core_overspeed);
    TEST_SUMMARY();
}
