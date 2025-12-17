#ifndef JETECU_TYPES_H
#define JETECU_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/ecu_types.h"
#include "core/ecu_sm.h"

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <cstdint>

namespace jetecu {

enum class State {
    Off       = ECU_STATE_OFF,
    Prestart  = ECU_STATE_PRESTART,
    Spinup    = ECU_STATE_SPINUP,
    Ignition  = ECU_STATE_IGNITION,
    Ramp      = ECU_STATE_RAMP,
    Run       = ECU_STATE_RUN,
    Cooldown  = ECU_STATE_COOLDOWN,
    Shutdown  = ECU_STATE_SHUTDOWN,
    Fault     = ECU_STATE_FAULT,
};

struct Inputs {
    float rpm      = 0.0f;
    float egt      = 0.0f;
    float throttle = 0.0f;
};

struct Outputs {
    float    fuel_pct    = 0.0f;
    bool     igniter_on  = false;
    bool     starter_on  = false;
    State    state       = State::Off;
    uint32_t fault_code  = 0;
};

} // namespace jetecu

#endif // __cplusplus
#endif // JETECU_TYPES_H
