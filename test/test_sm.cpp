#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_sm.h"
}

TEST(StateMachine, Init) {
    ecu_sm_t sm;
    ecu_sm_init(&sm);
    EXPECT_EQ(ecu_sm_state(&sm), ECU_STATE_OFF);
}

TEST(StateMachine, ValidTransition) {
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_PRESTART, 100);
    EXPECT_EQ(r, SM_TRANSITION);
    EXPECT_EQ(ecu_sm_state(&sm), ECU_STATE_PRESTART);
}

TEST(StateMachine, InvalidTransition) {
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_RUN, 100);
    EXPECT_EQ(r, SM_NO_CHANGE);
    EXPECT_EQ(ecu_sm_state(&sm), ECU_STATE_OFF);
}

TEST(StateMachine, FaultFromSpinup) {
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    ecu_sm_transition(&sm, ECU_STATE_PRESTART, 100);
    ecu_sm_transition(&sm, ECU_STATE_SPINUP, 200);
    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_FAULT, 300);
    EXPECT_EQ(r, SM_TRANSITION);
    EXPECT_EQ(ecu_sm_state(&sm), ECU_STATE_FAULT);
}

TEST(StateMachine, FullSequence) {
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_PRESTART, 100), SM_TRANSITION);
    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_SPINUP, 200), SM_TRANSITION);
    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_IGNITION, 300), SM_TRANSITION);
    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_RAMP, 400), SM_TRANSITION);
    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_RUN, 500), SM_TRANSITION);
    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_COOLDOWN, 600), SM_TRANSITION);
    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_SHUTDOWN, 700), SM_TRANSITION);
    EXPECT_EQ(ecu_sm_transition(&sm, ECU_STATE_OFF, 800), SM_TRANSITION);
}

TEST(StateMachine, FaultToShutdown) {
    ecu_sm_t sm;
    ecu_sm_init(&sm);

    ecu_sm_transition(&sm, ECU_STATE_PRESTART, 100);
    ecu_sm_transition(&sm, ECU_STATE_SPINUP, 200);
    ecu_sm_transition(&sm, ECU_STATE_FAULT, 300);

    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_SHUTDOWN, 400);
    EXPECT_EQ(r, SM_TRANSITION);
    EXPECT_EQ(ecu_sm_state(&sm), ECU_STATE_SHUTDOWN);
}

TEST(StateMachine, StateNames) {
    EXPECT_STREQ(ecu_sm_state_name(ECU_STATE_OFF), "OFF");
    EXPECT_STREQ(ecu_sm_state_name(ECU_STATE_PRESTART), "PRESTART");
    EXPECT_STREQ(ecu_sm_state_name(ECU_STATE_RAMP), "RAMP");
    EXPECT_STREQ(ecu_sm_state_name(ECU_STATE_COOLDOWN), "COOLDOWN");
    EXPECT_STREQ(ecu_sm_state_name(ECU_STATE_SHUTDOWN), "SHUTDOWN");
    EXPECT_STREQ(ecu_sm_state_name(ECU_STATE_FAULT), "FAULT");
}

TEST(StateMachine, SameStateNoop) {
    ecu_sm_t sm;
    ecu_sm_init(&sm);
    sm_result_t r = ecu_sm_transition(&sm, ECU_STATE_OFF, 100);
    EXPECT_EQ(r, SM_NO_CHANGE);
}

/* ---- C++ wrapper tests ---- */

#include "cpp/jetecu/StateMachine.h"

TEST(StateMachineWrapper, Init) {
    jetecu::StateMachine sm;
    EXPECT_EQ(sm.state(), jetecu::State::Off);
}

TEST(StateMachineWrapper, Transition) {
    jetecu::StateMachine sm;
    EXPECT_TRUE(sm.transition(jetecu::State::Prestart, 100));
    EXPECT_EQ(sm.state(), jetecu::State::Prestart);
}

TEST(StateMachineWrapper, InvalidTransition) {
    jetecu::StateMachine sm;
    EXPECT_FALSE(sm.transition(jetecu::State::Run, 100));
    EXPECT_EQ(sm.state(), jetecu::State::Off);
}

TEST(StateMachineWrapper, Callbacks) {
    jetecu::StateMachine sm;
    jetecu::State entered = jetecu::State::Off;
    jetecu::State exited = jetecu::State::Off;

    sm.setCallbacks(
        [&](jetecu::State s) { entered = s; },
        [&](jetecu::State s) { exited = s; }
    );

    sm.transition(jetecu::State::Prestart, 100);
    EXPECT_EQ(entered, jetecu::State::Prestart);
    EXPECT_EQ(exited, jetecu::State::Off);
}
