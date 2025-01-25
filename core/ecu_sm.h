#ifndef ECU_SM_H
#define ECU_SM_H

#include "core/ecu_types.h"

/* Engine states */
typedef enum {
    ECU_STATE_OFF = 0,
    ECU_STATE_PRESTART,
    ECU_STATE_SPINUP,
    ECU_STATE_IGNITION,
    ECU_STATE_RAMP,
    ECU_STATE_RUN,
    ECU_STATE_COOLDOWN,
    ECU_STATE_SHUTDOWN,
    ECU_STATE_FAULT,
} ecu_state_t;

#define ECU_STATE_COUNT 9

/* Transition result */
typedef enum {
    SM_NO_CHANGE = 0,
    SM_TRANSITION,
} sm_result_t;

/* State machine context */
typedef struct {
    ecu_state_t current;
    ecu_time_t  state_enter_time;
} ecu_sm_t;

/* Initialize state machine */
void ecu_sm_init(ecu_sm_t *sm);

/* Attempt state transition */
sm_result_t ecu_sm_transition(ecu_sm_t *sm, ecu_state_t next, ecu_time_t now);

/* Get current state */
ecu_state_t ecu_sm_state(const ecu_sm_t *sm);

/* Get name of state */
const char *ecu_sm_state_name(ecu_state_t state);

#endif /* ECU_SM_H */
