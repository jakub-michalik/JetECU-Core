#ifndef JETECU_STATE_MACHINE_H
#define JETECU_STATE_MACHINE_H

#include "cpp/jetecu/Types.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_sm.h"
#ifdef __cplusplus
}
#endif

#include <functional>

namespace jetecu {

class StateMachine {
public:
    using Callback = std::function<void(State)>;

    StateMachine();

    State state() const;
    const char *stateName() const;
    bool transition(State next, uint32_t now);
    bool timedOut(uint32_t now) const;
    void setTimeout(uint32_t ms);
    void setCallbacks(Callback on_enter, Callback on_exit);

    ecu_sm_t &raw() { return sm_; }
    const ecu_sm_t &raw() const { return sm_; }

private:
    ecu_sm_t sm_;
    Callback on_enter_;
    Callback on_exit_;

    static void enterTrampoline(ecu_state_t state, void *ctx);
    static void exitTrampoline(ecu_state_t state, void *ctx);
};

} // namespace jetecu

#endif // JETECU_STATE_MACHINE_H
