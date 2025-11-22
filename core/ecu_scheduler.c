#include "core/ecu_scheduler.h"
#include <string.h>

void ecu_sched_init(ecu_scheduler_t *sched)
{
    memset(sched, 0, sizeof(*sched));
}

int ecu_sched_add(ecu_scheduler_t *sched, const char *name,
                  sched_task_fn fn, void *ctx,
                  sched_priority_t prio, uint32_t interval_ms)
{
    if (sched->task_count >= SCHED_MAX_TASKS) return -1;

    sched_task_t *t = &sched->tasks[sched->task_count];
    t->name = name;
    t->fn = fn;
    t->ctx = ctx;
    t->priority = prio;
    t->interval_ms = interval_ms;
    t->last_run_ms = 0;
    t->worst_case_us = 0;
    t->run_count = 0;
    t->overrun = false;

    sched->task_count++;

    /* Sort by priority (bubble sort — only called at init) */
    for (int i = 0; i < sched->task_count - 1; i++) {
        for (int j = 0; j < sched->task_count - i - 1; j++) {
            if (sched->tasks[j].priority > sched->tasks[j + 1].priority) {
                sched_task_t tmp = sched->tasks[j];
                sched->tasks[j] = sched->tasks[j + 1];
                sched->tasks[j + 1] = tmp;
            }
        }
    }

    return 0;
}

void ecu_sched_tick(ecu_scheduler_t *sched, uint32_t now_ms)
{
    sched->tick_ms = now_ms;

    for (int i = 0; i < sched->task_count; i++) {
        sched_task_t *t = &sched->tasks[i];
        uint32_t elapsed = now_ms - t->last_run_ms;

        if (elapsed >= t->interval_ms) {
            uint32_t start = now_ms;  /* In real impl, use microsecond timer */
            t->fn(t->ctx);
            uint32_t end = now_ms;    /* Would need hal_timer_micros() */
            uint32_t duration_us = (end - start) * 1000;  /* rough estimate */

            if (duration_us > t->worst_case_us) {
                t->worst_case_us = duration_us;
            }

            /* Check for overrun (task took longer than its interval) */
            if (duration_us > t->interval_ms * 1000) {
                t->overrun = true;
            }

            t->last_run_ms = now_ms;
            t->run_count++;
        }
    }
}

uint32_t ecu_sched_get_wcet(const ecu_scheduler_t *sched, int task_idx)
{
    if (task_idx < 0 || task_idx >= sched->task_count) return 0;
    return sched->tasks[task_idx].worst_case_us;
}

bool ecu_sched_any_overrun(const ecu_scheduler_t *sched)
{
    for (int i = 0; i < sched->task_count; i++) {
        if (sched->tasks[i].overrun) return true;
    }
    return false;
}
