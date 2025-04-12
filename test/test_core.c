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

/* Helper: step multiple times with same inputs to let sensor filters settle */
static ecu_outputs_t step_n(ecu_t *ecu, ecu_inputs_t *in, float dt, int n)
{
    ecu_outputs_t out = {0};
    for (int i = 0; i < n; i++) {
        out = ecu_step(ecu, in, dt);
    }
    return out;
}

/* Helper: advance through startup phases */
static void advance_to_spinup(ecu_t *ecu, ecu_inputs_t *in)
{
    in->throttle = 50.0f;
    in->rpm = 0;
    in->egt = 25.0f;
    ecu_step(ecu, in, 0.01f);  /* OFF -> PRESTART */
    ecu_step(ecu, in, 0.01f);  /* PRESTART -> SPINUP */
}

static void advance_to_ignition(ecu_t *ecu, ecu_inputs_t *in)
{
    advance_to_spinup(ecu, in);
    in->rpm = 15000.0f;
    /* Multiple steps to let rate limiter pass the value through */
    step_n(ecu, in, 0.1f, 5);
}

static void advance_to_ramp(ecu_t *ecu, ecu_inputs_t *in)
{
    advance_to_ignition(ecu, in);
    in->egt = 200.0f;
    step_n(ecu, in, 0.1f, 3);
}

static void advance_to_run(ecu_t *ecu, ecu_inputs_t *in)
{
    advance_to_ramp(ecu, in);
    in->rpm = 58000.0f;
    step_n(ecu, in, 0.1f, 20);
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
    ecu_inputs_t in = {0};

    advance_to_spinup(&ecu, &in);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_SPINUP);

    in.rpm = 15000.0f;
    step_n(&ecu, &in, 0.1f, 5);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_IGNITION);
}

TEST(test_core_ignition_to_ramp)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = {0};

    advance_to_ignition(&ecu, &in);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_IGNITION);

    in.egt = 200.0f;
    step_n(&ecu, &in, 0.1f, 3);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_RAMP);
}

TEST(test_core_fault_on_timeout)
{
    ecu_t ecu = make_ecu();
    ecu.config.spinup_timeout_ms = 100;

    ecu_inputs_t in = { .rpm = 0, .egt = 25.0f, .throttle = 50.0f };
    ecu_step(&ecu, &in, 0.01f);
    ecu_step(&ecu, &in, 0.01f);

    in.rpm = 100.0f;
    for (int i = 0; i < 30; i++) {
        ecu_step(&ecu, &in, 0.01f);
    }
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.01f);
    ASSERT_EQ(out.state, ECU_STATE_FAULT);
}

TEST(test_core_egt_overtemp)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = {0};

    advance_to_ramp(&ecu, &in);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_RAMP);

    in.egt = 1200.0f;
    step_n(&ecu, &in, 0.1f, 15);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_FAULT);
}

TEST(test_core_overspeed)
{
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = {0};

    advance_to_run(&ecu, &in);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_RUN);

    in.rpm = 130000.0f;
    step_n(&ecu, &in, 0.1f, 20);
    ASSERT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_FAULT);
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
