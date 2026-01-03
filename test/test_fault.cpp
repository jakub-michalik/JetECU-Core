#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_fault.h"
}

TEST(Fault, Report) {
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_NONE, 100);
    EXPECT_EQ(ecu_fault_count(&mgr), 1);
    EXPECT_TRUE(ecu_fault_is_active(&mgr, 0x01));
}

TEST(Fault, Clear) {
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_NONE, 100);
    ecu_fault_clear(&mgr, 0x01);
    EXPECT_EQ(ecu_fault_count(&mgr), 0);
    EXPECT_FALSE(ecu_fault_is_active(&mgr, 0x01));
}

TEST(Fault, Latched) {
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x10, FAULT_SEV_CRITICAL, FAULT_ACTION_SHUTDOWN, 100);
    ecu_fault_clear(&mgr, 0x10);
    EXPECT_TRUE(ecu_fault_is_active(&mgr, 0x10));
}

TEST(Fault, Priority) {
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 100);
    EXPECT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_DEGRADE);

    ecu_fault_report(&mgr, 0x02, FAULT_SEV_CRITICAL, FAULT_ACTION_SHUTDOWN, 200);
    EXPECT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_SHUTDOWN);
}

TEST(Fault, Retry) {
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x05, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 100);
    ecu_fault_set_retries(&mgr, 0x05, 3);

    EXPECT_TRUE(ecu_fault_try_retry(&mgr, 0x05));
    EXPECT_FALSE(ecu_fault_is_active(&mgr, 0x05));

    ecu_fault_report(&mgr, 0x05, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 200);
    ecu_fault_set_retries(&mgr, 0x05, 3);
}

TEST(Fault, Log) {
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_NONE, 100);
    ecu_fault_report(&mgr, 0x02, FAULT_SEV_CRITICAL, FAULT_ACTION_SHUTDOWN, 200);
    EXPECT_GE(mgr.log_count, 2);
}

TEST(Fault, DegradeMode) {
    ecu_fault_mgr_t mgr;
    ecu_fault_init(&mgr);

    ecu_fault_report(&mgr, 0x01, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 100);
    EXPECT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_DEGRADE);

    ecu_fault_clear(&mgr, 0x01);
    EXPECT_EQ(ecu_fault_get_action(&mgr), FAULT_ACTION_NONE);
}

/* ---- C++ wrapper tests ---- */

#include "cpp/jetecu/FaultManager.h"

TEST(FaultManagerWrapper, ReportAndClear) {
    jetecu::FaultManager fm;
    fm.report(0x01, FAULT_SEV_WARNING, FAULT_ACTION_NONE, 100);
    EXPECT_EQ(fm.count(), 1);
    EXPECT_TRUE(fm.isActive(0x01));

    fm.clear(0x01);
    EXPECT_EQ(fm.count(), 0);
}

TEST(FaultManagerWrapper, Action) {
    jetecu::FaultManager fm;
    fm.report(0x01, FAULT_SEV_WARNING, FAULT_ACTION_DEGRADE, 100);
    EXPECT_EQ(fm.action(), FAULT_ACTION_DEGRADE);
}
