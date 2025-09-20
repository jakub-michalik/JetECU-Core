#ifndef ECU_FAULT_H
#define ECU_FAULT_H

#include "core/ecu_types.h"
#include <stdbool.h>

#define FAULT_MAX_ACTIVE  16
#define FAULT_LOG_SIZE    32

typedef enum {
    FAULT_SEV_INFO = 0,
    FAULT_SEV_WARNING,
    FAULT_SEV_CRITICAL,
    FAULT_SEV_FATAL,
} ecu_fault_severity_t;

typedef enum {
    FAULT_ACTION_NONE = 0,
    FAULT_ACTION_DEGRADE,    /* reduce power */
    FAULT_ACTION_SHUTDOWN,   /* safe shutdown */
} ecu_fault_action_t;

typedef struct {
    uint16_t             code;
    ecu_fault_severity_t severity;
    ecu_fault_action_t   action;
    ecu_time_t           timestamp;
    bool                 latched;
    bool                 active;
    uint8_t              retry_count;
    uint8_t              max_retries;
} ecu_fault_t;

typedef struct {
    ecu_fault_t  active[FAULT_MAX_ACTIVE];
    int          active_count;
    ecu_fault_t  log[FAULT_LOG_SIZE];
    int          log_head;
    int          log_count;
    ecu_fault_action_t highest_action;
} ecu_fault_mgr_t;

void ecu_fault_init(ecu_fault_mgr_t *mgr);

/* Report a fault */
void ecu_fault_report(ecu_fault_mgr_t *mgr, uint16_t code,
                      ecu_fault_severity_t severity,
                      ecu_fault_action_t action,
                      ecu_time_t now);

/* Clear a specific fault (if not latched) */
void ecu_fault_clear(ecu_fault_mgr_t *mgr, uint16_t code);

/* Check if any fault requires specific action */
ecu_fault_action_t ecu_fault_get_action(const ecu_fault_mgr_t *mgr);

/* Check if a specific fault is active */
bool ecu_fault_is_active(const ecu_fault_mgr_t *mgr, uint16_t code);

/* Get active fault count */
int ecu_fault_count(const ecu_fault_mgr_t *mgr);

/* Try retry for a retriable fault. Returns true if retry allowed. */
bool ecu_fault_try_retry(ecu_fault_mgr_t *mgr, uint16_t code);

/* Set max retries for a fault code */
void ecu_fault_set_retries(ecu_fault_mgr_t *mgr, uint16_t code, uint8_t max_retries);

#endif /* ECU_FAULT_H */
