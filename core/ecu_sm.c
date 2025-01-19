#include "core/ecu_sm.h"

/* Valid transitions: from -> to */
static const struct {
    ecu_state_t from;
    ecu_state_t to;
} valid_transitions[] = {
    { ECU_STATE_OFF,       ECU_STATE_PRESTART },
    { ECU_STATE_PRESTART,  ECU_STATE_SPINUP   },
    { ECU_STATE_PRESTART,  ECU_STATE_FAULT    },
    { ECU_STATE_SPINUP,    ECU_STATE_IGNITION },
    { ECU_STATE_SPINUP,    ECU_STATE_FAULT    },
    { ECU_STATE_IGNITION,  ECU_STATE_RUN      },
    { ECU_STATE_IGNITION,  ECU_STATE_FAULT    },
    { ECU_STATE_RUN,       ECU_STATE_FAULT    },
};

#define NUM_TRANSITIONS (sizeof(valid_transitions) / sizeof(valid_transitions[0]))

void ecu_sm_init(ecu_sm_t *sm)
{
    sm->current = ECU_STATE_OFF;
    sm->state_enter_time = 0;
}

sm_result_t ecu_sm_transition(ecu_sm_t *sm, ecu_state_t next, ecu_time_t now)
{
    if (next == sm->current) {
        return SM_NO_CHANGE;
    }

    for (size_t i = 0; i < NUM_TRANSITIONS; i++) {
        if (valid_transitions[i].from == sm->current &&
            valid_transitions[i].to == next) {
            sm->current = next;
            sm->state_enter_time = now;
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
    "OFF",
    "PRESTART",
    "SPINUP",
    "IGNITION",
    "RUN",
    "FAULT",
};

const char *ecu_sm_state_name(ecu_state_t state)
{
    if ((int)state < ECU_STATE_COUNT) {
        return state_names[(int)state];
    }
    return "UNKNOWN";
}
