#include "cpp/jetecu/StateMachine.h"

namespace jetecu {

StateMachine::StateMachine()
{
    ecu_sm_init(&sm_);
}

State StateMachine::state() const
{
    return static_cast<State>(ecu_sm_state(&sm_));
}

const char *StateMachine::stateName() const
{
    return ecu_sm_state_name(ecu_sm_state(&sm_));
}

bool StateMachine::transition(State next, uint32_t now)
{
    sm_result_t r = ecu_sm_transition(&sm_, static_cast<ecu_state_t>(next), now);
    return r == SM_TRANSITION;
}

bool StateMachine::timedOut(uint32_t now) const
{
    return ecu_sm_timed_out(&sm_, now);
}

void StateMachine::setTimeout(uint32_t ms)
{
    ecu_sm_set_timeout(&sm_, ms);
}

void StateMachine::setCallbacks(Callback on_enter, Callback on_exit)
{
    on_enter_ = std::move(on_enter);
    on_exit_ = std::move(on_exit);
    ecu_sm_set_callbacks(&sm_, enterTrampoline, exitTrampoline, this);
}

void StateMachine::enterTrampoline(ecu_state_t state, void *ctx)
{
    auto *self = static_cast<StateMachine *>(ctx);
    if (self->on_enter_) self->on_enter_(static_cast<State>(state));
}

void StateMachine::exitTrampoline(ecu_state_t state, void *ctx)
{
    auto *self = static_cast<StateMachine *>(ctx);
    if (self->on_exit_) self->on_exit_(static_cast<State>(state));
}

} // namespace jetecu
