#include "test/test_runner.h"
#include "core/ecu_fault.h"

TEST(test_fault_report)
{
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_NONE, 100);
    ASSERT_EQ(ecu_fault_count(&mgr), 1);
    ASSERT(ecu_fault_is_active(&mgr, 0x01));
}

TEST(test_fault_clear)
{
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_NONE, 100);
    ecu_fault_clear(&mgr, 0x01);
    ASSERT_EQ(ecu_fault_count(&mgr), 0);
    ASSERT(!ecu_fault_is_active(&mgr, 0x01));
}

TEST(test_fault_latched)
{
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    /* Critical faults are latched */
    ecu_fault_report(&mgr, 0x10, FAULT_SEV_CRITICAL, FAULT_ACTION_SHUTDOWN, 100);
    ecu_fault_clear(&mgr, 0x10);  /* should not clear */
    ASSERT(ecu_fault_is_active(&mgr, 0x10));
}

TEST(test_fault_priority)
{
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 100);
    ASSERT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_DEGRADE);

    ecu_fault_report(&mgr, 0x02, FAULT_SEV_CRITICAL, FAULT_ACTION_SHUTDOWN, 200);
    ASSERT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_SHUTDOWN);
}

TEST(test_fault_retry)
{
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x05, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 100);
    ecu_fault_set_retries(&mgr, 0x05, 3);

    ASSERT(ecu_fault_try_retry(&mgr, 0x05));
    ASSERT(!ecu_fault_is_active(&mgr, 0x05));

    /* Report again, retry again */
    ecu_fault_report(&mgr, 0x05, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 200);
    ecu_fault_set_retries(&mgr, 0x05, 3);
}

TEST(test_fault_log)
{
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_NONE, 100);
    ecu_fault_report(&mgr, 0x02, FAULT_SEV_CRITICAL, FAULT_ACTION_SHUTDOWN, 200);
    ASSERT(mgr.log_count >= 2);
}

TEST(test_fault_degrade_mode)
{
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 100);
    ASSERT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_DEGRADE);

    /* Clear it */
    ecu_fault_clear(&mgr, 0x01);
    ASSERT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_NONE);
}

int main(void)
{
    printf("=== Fault Tests ===\n");
    RUN_TEST(test_fault_report);
    RUN_TEST(test_fault_clear);
    RUN_TEST(test_fault_latched);
    RUN_TEST(test_fault_priority);
    RUN_TEST(test_fault_retry);
    RUN_TEST(test_fault_log);
    RUN_TEST(test_fault_degrade_mode);
    TEST_SUMMARY();
}
