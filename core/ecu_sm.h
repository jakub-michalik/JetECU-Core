#ifndef ECU_SM_H
#define ECU_SM_H

#include "core/ecu_types.h"

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

typedef enum {
    SM_NO_CHANGE = 0,
    SM_TRANSITION,
} sm_result_t;

/* Callback type for state entry/exit */
typedef void (*ecu_sm_callback_t)(ecu_state_t state, void *ctx);

typedef struct {
    ecu_state_t        current;
    ecu_time_t         state_enter_time;
    ecu_time_t         timeout_ms;       /* per-state timeout (0 = none) */
    ecu_sm_callback_t  on_enter;
    ecu_sm_callback_t  on_exit;
    void              *cb_ctx;
} ecu_sm_t;

void ecu_sm_init(ecu_sm_t *sm);
sm_result_t ecu_sm_transition(ecu_sm_t *sm, ecu_state_t next, ecu_time_t now);
ecu_state_t ecu_sm_state(const ecu_sm_t *sm);
const char *ecu_sm_state_name(ecu_state_t state);

/* Set callbacks for state transitions */
void ecu_sm_set_callbacks(ecu_sm_t *sm, ecu_sm_callback_t on_enter,
                          ecu_sm_callback_t on_exit, void *ctx);

/* Set timeout for current state */
void ecu_sm_set_timeout(ecu_sm_t *sm, ecu_time_t timeout_ms);

/* Check if current state has timed out */
bool ecu_sm_timed_out(const ecu_sm_t *sm, ecu_time_t now);

#endif /* ECU_SM_H */
