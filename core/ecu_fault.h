/**
 * @file ecu_fault.h
 * @brief Fault management with severity, latching, and retry policies.
 */

#ifndef ECU_FAULT_H
#define ECU_FAULT_H

#include "core/ecu_types.h"
#include <stdbool.h>

/**
 * @defgroup ecu_fault Fault Manager
 * @brief Priority-based fault reporting, latching, retry, and degrade mode.
 * @{
 */

#define FAULT_MAX_ACTIVE  16  /**< Maximum concurrently active faults. */
#define FAULT_LOG_SIZE    32  /**< Circular log capacity. */

/** @brief Fault severity levels (ascending priority). */
typedef enum {
    FAULT_SEV_INFO = 0,      /**< Informational — logged only. */
    FAULT_SEV_WARNING,       /**< Warning — triggers degrade mode. */
    FAULT_SEV_CRITICAL,      /**< Critical — latched, requires shutdown. */
    FAULT_SEV_FATAL,         /**< Fatal — immediate safe shutdown. */
} ecu_fault_severity_t;

/** @brief System-level action required by a fault. */
typedef enum {
    FAULT_ACTION_NONE = 0,   /**< No action required. */
    FAULT_ACTION_DEGRADE,    /**< Reduce power / limit RPM. */
    FAULT_ACTION_SHUTDOWN,   /**< Initiate safe shutdown sequence. */
} ecu_fault_action_t;

/** @brief Single fault record. */
typedef struct {
    uint16_t             code;         /**< Unique fault code. */
    ecu_fault_severity_t severity;     /**< Severity classification. */
    ecu_fault_action_t   action;       /**< Required action. */
    ecu_time_t           timestamp;    /**< Time of first report (ms). */
    bool                 latched;      /**< Cannot be cleared without reset. */
    bool                 active;       /**< Currently active. */
    uint8_t              retry_count;  /**< Retries attempted so far. */
    uint8_t              max_retries;  /**< Maximum allowed retries. */
} ecu_fault_t;

/** @brief Fault manager context. */
typedef struct {
    ecu_fault_t  active[FAULT_MAX_ACTIVE]; /**< Active fault slots. */
    int          active_count;             /**< Number of active faults. */
    ecu_fault_t  log[FAULT_LOG_SIZE];      /**< Circular fault log. */
    int          log_head;                 /**< Next write position in log. */
    int          log_count;                /**< Total entries logged. */
    ecu_fault_action_t highest_action;     /**< Highest priority action needed. */
} ecu_fault_mgr_t;

/**
 * @brief Initialise the fault manager.
 * @param[out] mgr  Fault manager context.
 */
void ecu_fault_init(ecu_fault_mgr_t *mgr);

/**
 * @brief Report a fault.
 *
 * @param[in,out] mgr       Fault manager context.
 * @param[in]     code      Fault code.
 * @param[in]     severity  Severity level.
 * @param[in]     action    Required action.
 * @param[in]     now       Current timestamp (ms).
 */
void ecu_fault_report(ecu_fault_mgr_t *mgr, uint16_t code,
                      ecu_fault_severity_t severity,
                      ecu_fault_action_t action,
                      ecu_time_t now);

/**
 * @brief Clear a non-latched fault.
 * @param[in,out] mgr   Fault manager context.
 * @param[in]     code  Fault code to clear.
 */
void ecu_fault_clear(ecu_fault_mgr_t *mgr, uint16_t code);

/**
 * @brief Get the highest-priority action required.
 * @param[in] mgr  Fault manager context.
 * @return Highest action across all active faults.
 */
ecu_fault_action_t ecu_fault_get_action(const ecu_fault_mgr_t *mgr);

/**
 * @brief Check whether a specific fault is active.
 *
 * @param[in] mgr   Fault manager context.
 * @param[in] code  Fault code.
 * @return @c true if active.
 */
bool ecu_fault_is_active(const ecu_fault_mgr_t *mgr, uint16_t code);

/**
 * @brief Get the number of active faults.
 * @param[in] mgr  Fault manager context.
 * @return Active fault count.
 */
int ecu_fault_count(const ecu_fault_mgr_t *mgr);

/**
 * @brief Attempt a retry for a retriable fault.
 *
 * @param[in,out] mgr   Fault manager context.
 * @param[in]     code  Fault code.
 * @return @c true if a retry is permitted.
 */
bool ecu_fault_try_retry(ecu_fault_mgr_t *mgr, uint16_t code);

/**
 * @brief Set the maximum retry count for a fault code.
 *
 * @param[in,out] mgr          Fault manager context.
 * @param[in]     code         Fault code.
 * @param[in]     max_retries  Maximum retry attempts.
 */
void ecu_fault_set_retries(ecu_fault_mgr_t *mgr, uint16_t code, uint8_t max_retries);

/** @} */ /* end ecu_fault */

#endif /* ECU_FAULT_H */
