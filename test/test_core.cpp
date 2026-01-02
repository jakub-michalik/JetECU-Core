#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_core.h"
}

static ecu_t make_ecu()
{
    ecu_t ecu;
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);
    return ecu;
}

static ecu_outputs_t step_n(ecu_t *ecu, ecu_inputs_t *in, float dt, int n)
{
    ecu_outputs_t out = {};
    for (int i = 0; i < n; i++) {
        out = ecu_step(ecu, in, dt);
    }
    return out;
}

static void advance_to_spinup(ecu_t *ecu, ecu_inputs_t *in)
{
    in->throttle = 50.0f;
    in->rpm = 0;
    in->egt = 25.0f;
    ecu_step(ecu, in, 0.01f);
    ecu_step(ecu, in, 0.01f);
}

static void advance_to_ignition(ecu_t *ecu, ecu_inputs_t *in)
{
    advance_to_spinup(ecu, in);
    in->rpm = 15000.0f;
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

TEST(Core, Init) {
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = { 0, 25.0f, 0 };
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.01f);
    EXPECT_EQ(out.state, ECU_STATE_OFF);
    EXPECT_LT(out.fuel_pct, 0.01f);
}

TEST(Core, StartSequence) {
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = {};

    advance_to_spinup(&ecu, &in);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_SPINUP);

    in.rpm = 15000.0f;
    step_n(&ecu, &in, 0.1f, 5);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_IGNITION);
}

TEST(Core, IgnitionToRamp) {
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = {};

    advance_to_ignition(&ecu, &in);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_IGNITION);

    in.egt = 200.0f;
    step_n(&ecu, &in, 0.1f, 3);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_RAMP);
}

TEST(Core, FaultOnTimeout) {
    ecu_t ecu = make_ecu();
    ecu.config.spinup_timeout_ms = 100;

    ecu_inputs_t in = { 0, 25.0f, 50.0f };
    ecu_step(&ecu, &in, 0.01f);
    ecu_step(&ecu, &in, 0.01f);

    in.rpm = 100.0f;
    for (int i = 0; i < 30; i++) {
        ecu_step(&ecu, &in, 0.01f);
    }
    ecu_outputs_t out = ecu_step(&ecu, &in, 0.01f);
    EXPECT_EQ(out.state, ECU_STATE_FAULT);
}

TEST(Core, EgtOvertemp) {
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = {};

    advance_to_ramp(&ecu, &in);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_RAMP);

    in.egt = 1200.0f;
    step_n(&ecu, &in, 0.1f, 15);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_FAULT);
}

TEST(Core, Overspeed) {
    ecu_t ecu = make_ecu();
    ecu_inputs_t in = {};

    advance_to_run(&ecu, &in);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_RUN);

    in.rpm = 130000.0f;
    step_n(&ecu, &in, 0.1f, 20);
    EXPECT_EQ(ecu_sm_state(&ecu.sm), ECU_STATE_FAULT);
}

/* ---- C++ wrapper tests ---- */

#include "cpp/jetecu/Engine.h"

TEST(EngineWrapper, DefaultConstruction) {
    jetecu::Engine engine;
    EXPECT_EQ(engine.state(), jetecu::State::Off);
}

TEST(EngineWrapper, StepReturnsOutputs) {
    jetecu::Engine engine;
    jetecu::Inputs in;
    auto out = engine.step(in, 0.01f);
    EXPECT_EQ(out.state, jetecu::State::Off);
    EXPECT_NEAR(out.fuel_pct, 0.0f, 0.01f);
}

TEST(EngineWrapper, StartSequence) {
    jetecu::Engine engine;
    jetecu::Inputs in;
    in.throttle = 50.0f;
    in.rpm = 0;
    in.egt = 25.0f;

    engine.step(in, 0.01f);
    engine.step(in, 0.01f);
    EXPECT_EQ(engine.state(), jetecu::State::Spinup);
}

TEST(EngineWrapper, StateName) {
    jetecu::Engine engine;
    EXPECT_STREQ(engine.stateName(), "OFF");
}
