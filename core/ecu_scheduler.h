#ifndef ECU_SCHEDULER_H
#define ECU_SCHEDULER_H

#include "core/ecu_types.h"

#define SCHED_MAX_TASKS 8

typedef void (*sched_task_fn)(void *ctx);

typedef enum {
    SCHED_PRIO_CRITICAL = 0,
    SCHED_PRIO_HIGH,
    SCHED_PRIO_MEDIUM,
    SCHED_PRIO_LOW,
} sched_priority_t;

typedef struct {
    const char      *name;
    sched_task_fn    fn;
    void            *ctx;
    sched_priority_t priority;
    uint32_t         interval_ms;    /* how often to run */
    uint32_t         last_run_ms;    /* last execution time */
    uint32_t         worst_case_us;  /* worst-case execution time */
    uint32_t         run_count;
    bool             overrun;        /* set if task took too long */
} sched_task_t;

typedef struct {
    sched_task_t tasks[SCHED_MAX_TASKS];
    int          task_count;
    uint32_t     tick_ms;
} ecu_scheduler_t;

/* Initialize scheduler */
void ecu_sched_init(ecu_scheduler_t *sched);

/* Add a task */
int ecu_sched_add(ecu_scheduler_t *sched, const char *name,
                  sched_task_fn fn, void *ctx,
                  sched_priority_t prio, uint32_t interval_ms);

/* Run one scheduler tick — executes due tasks in priority order */
void ecu_sched_tick(ecu_scheduler_t *sched, uint32_t now_ms);

/* Get worst-case execution time for a task (microseconds) */
uint32_t ecu_sched_get_wcet(const ecu_scheduler_t *sched, int task_idx);

/* Check if any task has overrun */
bool ecu_sched_any_overrun(const ecu_scheduler_t *sched);

#endif /* ECU_SCHEDULER_H */
