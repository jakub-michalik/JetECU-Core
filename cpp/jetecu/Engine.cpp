#include "cpp/jetecu/Engine.h"

namespace jetecu {

Engine::Engine()
{
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu_, &cfg);
}

Engine::Engine(const ecu_config_t &cfg)
{
    ecu_init(&ecu_, &cfg);
}

Outputs Engine::step(const Inputs &in, float dt)
{
    ecu_inputs_t cin = { in.rpm, in.egt, in.throttle };
    ecu_outputs_t cout = ecu_step(&ecu_, &cin, dt);

    Outputs out;
    out.fuel_pct   = cout.fuel_pct;
    out.igniter_on = cout.igniter_on;
    out.starter_on = cout.starter_on;
    out.state      = static_cast<State>(cout.state);
    out.fault_code = cout.fault_code;
    return out;
}

State Engine::state() const
{
    return static_cast<State>(ecu_sm_state(&ecu_.sm));
}

const char *Engine::stateName() const
{
    return ecu_sm_state_name(ecu_sm_state(&ecu_.sm));
}

} // namespace jetecu
