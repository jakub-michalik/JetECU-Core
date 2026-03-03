/**
 * @file ecu_scheduler.h
 * @brief Deterministic fixed-priority task scheduler.
 */

#ifndef ECU_SCHEDULER_H
#define ECU_SCHEDULER_H

#include "core/ecu_types.h"

/**
 * @defgroup ecu_scheduler Scheduler
 * @brief Fixed-priority cooperative scheduler with WCET tracking.
 * @{
 */

#define SCHED_MAX_TASKS 8  /**< Maximum registered tasks. */

/** @brief Task function signature. */
typedef void (*sched_task_fn)(void *ctx);

/** @brief Task priority levels (lower value = higher priority). */
typedef enum {
    SCHED_PRIO_CRITICAL = 0, /**< Safety-critical (e.g. control loop). */
    SCHED_PRIO_HIGH,         /**< High priority (e.g. watchdog). */
    SCHED_PRIO_MEDIUM,       /**< Medium priority (e.g. telemetry). */
    SCHED_PRIO_LOW,          /**< Low priority (e.g. housekeeping). */
} sched_priority_t;

/** @brief Registered task descriptor. */
typedef struct {
    const char      *name;          /**< Human-readable task name. */
    sched_task_fn    fn;            /**< Task function pointer. */
    void            *ctx;           /**< User context for @p fn. */
    sched_priority_t priority;      /**< Execution priority. */
    uint32_t         interval_ms;   /**< Execution interval (ms). */
    uint32_t         last_run_ms;   /**< Timestamp of last execution. */
    uint32_t         worst_case_us; /**< Worst-case execution time (us). */
    uint32_t         run_count;     /**< Total executions. */
    bool             overrun;       /**< Set if interval was exceeded. */
} sched_task_t;

/** @brief Scheduler context. */
typedef struct {
    sched_task_t tasks[SCHED_MAX_TASKS]; /**< Registered tasks. */
    int          task_count;             /**< Number of registered tasks. */
    uint32_t     tick_ms;                /**< Current tick time. */
} ecu_scheduler_t;

/**
 * @brief Initialise the scheduler.
 * @param[out] sched  Scheduler context.
 */
void ecu_sched_init(ecu_scheduler_t *sched);

/**
 * @brief Register a periodic task.
 *
 * @param[in,out] sched        Scheduler context.
 * @param[in]     name         Task name (pointer must remain valid).
 * @param[in]     fn           Task function.
 * @param[in]     ctx          User context passed to @p fn.
 * @param[in]     prio         Task priority.
 * @param[in]     interval_ms  Execution interval in milliseconds.
 * @return Task index on success, or -1 if the table is full.
 */
int ecu_sched_add(ecu_scheduler_t *sched, const char *name,
                  sched_task_fn fn, void *ctx,
                  sched_priority_t prio, uint32_t interval_ms);

/**
 * @brief Execute one scheduler tick.
 *
 * Runs all tasks whose interval has elapsed, in priority order.
 *
 * @param[in,out] sched   Scheduler context.
 * @param[in]     now_ms  Current time in milliseconds.
 */
void ecu_sched_tick(ecu_scheduler_t *sched, uint32_t now_ms);

/**
 * @brief Get worst-case execution time for a task.
 *
 * @param[in] sched     Scheduler context.
 * @param[in] task_idx  Task index (from ecu_sched_add()).
 * @return WCET in microseconds.
 */
uint32_t ecu_sched_get_wcet(const ecu_scheduler_t *sched, int task_idx);

/**
 * @brief Check if any task has experienced an overrun.
 *
 * @param[in] sched  Scheduler context.
 * @return @c true if any task has overrun its interval.
 */
bool ecu_sched_any_overrun(const ecu_scheduler_t *sched);

/** @} */ /* end ecu_scheduler */

#endif /* ECU_SCHEDULER_H */
