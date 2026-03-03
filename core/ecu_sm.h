/**
 * @file ecu_sm.h
 * @brief Engine state machine with timeout and callback support.
 */

#ifndef ECU_SM_H
#define ECU_SM_H

#include "core/ecu_types.h"

/**
 * @defgroup ecu_sm State Machine
 * @brief Deterministic state machine for the engine start/run/shutdown cycle.
 * @{
 */

/** @brief Engine operating states. */
typedef enum {
    ECU_STATE_OFF = 0,     /**< Controller inactive. */
    ECU_STATE_PRESTART,    /**< Sensor warmup / self-check. */
    ECU_STATE_SPINUP,      /**< Starter driving shaft to minimum RPM. */
    ECU_STATE_IGNITION,    /**< Igniter + first fuel injection. */
    ECU_STATE_RAMP,        /**< Accelerating to target RPM. */
    ECU_STATE_RUN,         /**< Stable operation under PID control. */
    ECU_STATE_COOLDOWN,    /**< Lowering EGT before shutdown. */
    ECU_STATE_SHUTDOWN,    /**< Engine stopped, outputs safe. */
    ECU_STATE_FAULT,       /**< Safety violation — awaiting reset. */
} ecu_state_t;

#define ECU_STATE_COUNT 9  /**< Total number of states. */

/** @brief Result of a state transition attempt. */
typedef enum {
    SM_NO_CHANGE = 0,      /**< State unchanged. */
    SM_TRANSITION,         /**< Transition occurred. */
} sm_result_t;

/** @brief Callback invoked on state entry or exit. */
typedef void (*ecu_sm_callback_t)(ecu_state_t state, void *ctx);

/** @brief State machine runtime context. */
typedef struct {
    ecu_state_t        current;          /**< Current state. */
    ecu_time_t         state_enter_time; /**< Timestamp of last entry. */
    ecu_time_t         timeout_ms;       /**< Per-state timeout (0 = none). */
    ecu_sm_callback_t  on_enter;         /**< Entry callback (may be NULL). */
    ecu_sm_callback_t  on_exit;          /**< Exit callback (may be NULL). */
    void              *cb_ctx;           /**< User context for callbacks. */
} ecu_sm_t;

/**
 * @brief Initialise the state machine (state = OFF).
 * @param[out] sm  State machine context.
 */
void ecu_sm_init(ecu_sm_t *sm);

/**
 * @brief Attempt a state transition.
 *
 * @param[in,out] sm    State machine context.
 * @param[in]     next  Target state.
 * @param[in]     now   Current timestamp (ms).
 * @return @c SM_TRANSITION on success, @c SM_NO_CHANGE otherwise.
 */
sm_result_t ecu_sm_transition(ecu_sm_t *sm, ecu_state_t next, ecu_time_t now);

/**
 * @brief Get the current state.
 * @param[in] sm  State machine context.
 * @return Current state.
 */
ecu_state_t ecu_sm_state(const ecu_sm_t *sm);

/**
 * @brief Get a human-readable name for the given state.
 * @param[in] state  State value.
 * @return Null-terminated state name string.
 */
const char *ecu_sm_state_name(ecu_state_t state);

/**
 * @brief Register entry/exit callbacks.
 *
 * @param[in,out] sm        State machine context.
 * @param[in]     on_enter  Called after entering a new state (may be NULL).
 * @param[in]     on_exit   Called before leaving the current state (may be NULL).
 * @param[in]     ctx       User context forwarded to callbacks.
 */
void ecu_sm_set_callbacks(ecu_sm_t *sm, ecu_sm_callback_t on_enter,
                          ecu_sm_callback_t on_exit, void *ctx);

/**
 * @brief Set a timeout for the current state.
 * @param[in,out] sm          State machine context.
 * @param[in]     timeout_ms  Timeout in milliseconds (0 = disable).
 */
void ecu_sm_set_timeout(ecu_sm_t *sm, ecu_time_t timeout_ms);

/**
 * @brief Check whether the current state has exceeded its timeout.
 *
 * @param[in] sm   State machine context.
 * @param[in] now  Current timestamp (ms).
 * @return @c true if timed out.
 */
bool ecu_sm_timed_out(const ecu_sm_t *sm, ecu_time_t now);

/** @} */ /* end ecu_sm */

#endif /* ECU_SM_H */
