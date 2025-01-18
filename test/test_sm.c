#include "test/test_runner.h"
#include "core/ecu_sm.h"

TEST(test_sm_init)
{
    ecu_sm_t sm;
    ecu_sm_init(&sm);
    ASSERT_EQ(ecu_sm_state(&sm), ECU_STATE_OFF);
}

TEST(test_sm_valid_transition)
{
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_PRESTART, 100);
    ASSERT_EQ(r, SM_TRANSITION);
    ASSERT_EQ(ecu_sm_state(&sm), ECU_STATE_PRESTART);
}

TEST(test_sm_invalid_transition)
{
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    /* Can't go directly from OFF to RUN */
    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_RUN, 100);
    ASSERT_EQ(r, SM_NO_CHANGE);
    ASSERT_EQ(ecu_sm_state(&sm), ECU_STATE_OFF);
}

TEST(test_sm_fault_from_spinup)
{
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    ecu_sm_transition(&sm, ECU_STATE_PRESTART, 100);
    ecu_sm_transition(&sm, ECU_STATE_SPINUP, 200);
    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_FAULT, 300);
    ASSERT_EQ(r, SM_TRANSITION);
    ASSERT_EQ(ecu_sm_state(&sm), ECU_STATE_FAULT);
}

TEST(test_sm_state_names)
{
    ASSERT_STR_EQ(ecu_sm_state_name(ECU_STATE_OFF), "OFF");
    ASSERT_STR_EQ(ecu_sm_state_name(ECU_STATE_PRESTART), "PRESTART");
    ASSERT_STR_EQ(ecu_sm_state_name(ECU_STATE_FAULT), "FAULT");
}

TEST(test_sm_same_state_noop)
{
    ecu_sm_t sm;
    ecu_sm_init(&sm);
    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_OFF, 100);
    ASSERT_EQ(r, SM_NO_CHANGE);
}

int main(void)
{
    printf("=== State Machine Tests ===\n");
    RUN_TEST(test_sm_init);
    RUN_TEST(test_sm_valid_transition);
    RUN_TEST(test_sm_invalid_transition);
    RUN_TEST(test_sm_fault_from_spinup);
    RUN_TEST(test_sm_state_names);
    RUN_TEST(test_sm_same_state_noop);
    TEST_SUMMARY();
}
