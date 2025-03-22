#include "core/ecu_sm.h"

static const struct {
    ecu_state_t from;
    ecu_state_t to;
} valid_transitions[] = {
    { ECU_STATE_OFF,       ECU_STATE_PRESTART  },
    { ECU_STATE_PRESTART,  ECU_STATE_SPINUP    },
    { ECU_STATE_PRESTART,  ECU_STATE_FAULT     },
    { ECU_STATE_SPINUP,    ECU_STATE_IGNITION  },
    { ECU_STATE_SPINUP,    ECU_STATE_FAULT     },
    { ECU_STATE_IGNITION,  ECU_STATE_RAMP      },
    { ECU_STATE_IGNITION,  ECU_STATE_FAULT     },
    { ECU_STATE_RAMP,      ECU_STATE_RUN       },
    { ECU_STATE_RAMP,      ECU_STATE_FAULT     },
    { ECU_STATE_RUN,       ECU_STATE_COOLDOWN  },
    { ECU_STATE_RUN,       ECU_STATE_FAULT     },
    { ECU_STATE_COOLDOWN,  ECU_STATE_SHUTDOWN  },
    { ECU_STATE_SHUTDOWN,  ECU_STATE_OFF       },
    { ECU_STATE_FAULT,     ECU_STATE_SHUTDOWN  },
};

#define NUM_TRANSITIONS (sizeof(valid_transitions) / sizeof(valid_transitions[0]))

void ecu_sm_init(ecu_sm_t *sm)
{
    sm->current = ECU_STATE_OFF;
    sm->state_enter_time = 0;
    sm->timeout_ms = 0;
    sm->on_enter = NULL;
    sm->on_exit = NULL;
    sm->cb_ctx = NULL;
}

void ecu_sm_set_callbacks(ecu_sm_t *sm, ecu_sm_callback_t on_enter,
                          ecu_sm_callback_t on_exit, void *ctx)
{
    sm->on_enter = on_enter;
    sm->on_exit = on_exit;
    sm->cb_ctx = ctx;
}

void ecu_sm_set_timeout(ecu_sm_t *sm, ecu_time_t timeout_ms)
{
    sm->timeout_ms = timeout_ms;
}

bool ecu_sm_timed_out(const ecu_sm_t *sm, ecu_time_t now)
{
    if (sm->timeout_ms == 0) return false;
    return (now - sm->state_enter_time) > sm->timeout_ms;
}

sm_result_t ecu_sm_transition(ecu_sm_t *sm, ecu_state_t next, ecu_time_t now)
{
    if (next == sm->current) {
        return SM_NO_CHANGE;
    }

    for (size_t i = 0; i < NUM_TRANSITIONS; i++) {
        if (valid_transitions[i].from == sm->current &&
            valid_transitions[i].to == next) {

            /* Exit callback */
            if (sm->on_exit) {
                sm->on_exit(sm->current, sm->cb_ctx);
            }

            sm->current = next;
            sm->state_enter_time = now;
            sm->timeout_ms = 0;  /* reset timeout for new state */

            /* Enter callback */
            if (sm->on_enter) {
                sm->on_enter(next, sm->cb_ctx);
            }

            return SM_TRANSITION;
        }
    }

    return SM_NO_CHANGE;
}

ecu_state_t ecu_sm_state(const ecu_sm_t *sm)
{
    return sm->current;
}

static const char *state_names[] = {
    "OFF", "PRESTART", "SPINUP", "IGNITION",
    "RAMP", "RUN", "COOLDOWN", "SHUTDOWN", "FAULT",
};

const char *ecu_sm_state_name(ecu_state_t state)
{
    if ((int)state < ECU_STATE_COUNT) {
        return state_names[(int)state];
    }
    return "UNKNOWN";
}
