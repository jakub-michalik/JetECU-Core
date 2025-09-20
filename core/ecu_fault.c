#include "core/ecu_fault.h"
#include <string.h>

void ecu_fault_init(ecu_fault_mgr_t *mgr)
{
    memset(mgr, 0, sizeof(*mgr));
    mgr->highest_action = FAULT_ACTION_NONE;
}

static ecu_fault_t *find_active(ecu_fault_mgr_t *mgr, uint16_t code)
{
    for (int i = 0; i < mgr->active_count; i++) {
        if (mgr->active[i].code == code && mgr->active[i].active) {
            return &mgr->active[i];
        }
    }
    return NULL;
}

static void update_highest_action(ecu_fault_mgr_t *mgr)
{
    mgr->highest_action = FAULT_ACTION_NONE;
    for (int i = 0; i < mgr->active_count; i++) {
        if (mgr->active[i].active && mgr->active[i].action > mgr->highest_action) {
            mgr->highest_action = mgr->active[i].action;
        }
    }
}

static void add_to_log(ecu_fault_mgr_t *mgr, const ecu_fault_t *fault)
{
    mgr->log[mgr->log_head] = *fault;
    mgr->log_head = (mgr->log_head + 1) % FAULT_LOG_SIZE;
    if (mgr->log_count < FAULT_LOG_SIZE) mgr->log_count++;
}

void ecu_fault_report(ecu_fault_mgr_t *mgr, uint16_t code,
                      ecu_fault_severity_t severity,
                      ecu_fault_action_t action,
                      ecu_time_t now)
{
    ecu_fault_t *existing = find_active(mgr, code);
    if (existing) {
        /* Already active — update timestamp */
        existing->timestamp = now;
        return;
    }

    if (mgr->active_count >= FAULT_MAX_ACTIVE) return;

    ecu_fault_t fault = {
        .code = code,
        .severity = severity,
        .action = action,
        .timestamp = now,
        .latched = (severity >= FAULT_SEV_CRITICAL),
        .active = true,
        .retry_count = 0,
        .max_retries = 0,
    };

    mgr->active[mgr->active_count++] = fault;
    add_to_log(mgr, &fault);
    update_highest_action(mgr);
}

void ecu_fault_clear(ecu_fault_mgr_t *mgr, uint16_t code)
{
    for (int i = 0; i < mgr->active_count; i++) {
        if (mgr->active[i].code == code && mgr->active[i].active) {
            if (mgr->active[i].latched) return;  /* can't clear latched */
            mgr->active[i].active = false;
            /* Compact array */
            for (int j = i; j < mgr->active_count - 1; j++) {
                mgr->active[j] = mgr->active[j + 1];
            }
            mgr->active_count--;
            update_highest_action(mgr);
            return;
        }
    }
}

ecu_fault_action_t ecu_fault_get_action(const ecu_fault_mgr_t *mgr)
{
    return mgr->highest_action;
}

bool ecu_fault_is_active(const ecu_fault_mgr_t *mgr, uint16_t code)
{
    for (int i = 0; i < mgr->active_count; i++) {
        if (mgr->active[i].code == code && mgr->active[i].active) {
            return true;
        }
    }
    return false;
}

int ecu_fault_count(const ecu_fault_mgr_t *mgr)
{
    return mgr->active_count;
}

bool ecu_fault_try_retry(ecu_fault_mgr_t *mgr, uint16_t code)
{
    ecu_fault_t *f = find_active(mgr, code);
    if (!f) return false;
    if (f->max_retries == 0) return false;
    if (f->retry_count >= f->max_retries) return false;

    f->retry_count++;
    f->active = false;
    /* Compact */
    int idx = (int)(f - mgr->active);
    for (int j = idx; j < mgr->active_count - 1; j++) {
        mgr->active[j] = mgr->active[j + 1];
    }
    mgr->active_count--;
    update_highest_action(mgr);
    return true;
}

void ecu_fault_set_retries(ecu_fault_mgr_t *mgr, uint16_t code, uint8_t max_retries)
{
    ecu_fault_t *f = find_active(mgr, code);
    if (f) f->max_retries = max_retries;
}
