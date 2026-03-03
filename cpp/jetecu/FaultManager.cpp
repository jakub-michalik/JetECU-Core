#include "cpp/jetecu/FaultManager.h"

namespace jetecu {

FaultManager::FaultManager()
{
    ecu_fault_init(&mgr_);
}

void FaultManager::report(uint16_t code, ecu_fault_severity_t severity,
                          ecu_fault_action_t action, uint32_t now)
{
    ecu_fault_report(&mgr_, code, severity, action, now);
}

void FaultManager::clear(uint16_t code)
{
    ecu_fault_clear(&mgr_, code);
}

bool FaultManager::isActive(uint16_t code) const
{
    return ecu_fault_is_active(&mgr_, code);
}

int FaultManager::count() const
{
    return ecu_fault_count(&mgr_);
}

ecu_fault_action_t FaultManager::action() const
{
    return ecu_fault_get_action(&mgr_);
}

bool FaultManager::tryRetry(uint16_t code)
{
    return ecu_fault_try_retry(&mgr_, code);
}

void FaultManager::setRetries(uint16_t code, uint8_t max_retries)
{
    ecu_fault_set_retries(&mgr_, code, max_retries);
}

ActiveFaultRange FaultManager::activeFaults() const
{
    return ActiveFaultRange(mgr_.active, mgr_.active_count);
}

LogRange FaultManager::log() const
{
    int entries = mgr_.log_count < FAULT_LOG_SIZE
                      ? mgr_.log_count : FAULT_LOG_SIZE;
    return LogRange(mgr_.log, entries);
}

int FaultManager::logCount() const
{
    return mgr_.log_count;
}

const char *FaultManager::severityName(ecu_fault_severity_t sev)
{
    switch (sev) {
    case FAULT_SEV_INFO:     return "INFO";
    case FAULT_SEV_WARNING:  return "WARNING";
    case FAULT_SEV_CRITICAL: return "CRITICAL";
    case FAULT_SEV_FATAL:    return "FATAL";
    }
    return "UNKNOWN";
}

const char *FaultManager::actionName(ecu_fault_action_t act)
{
    switch (act) {
    case FAULT_ACTION_NONE:     return "NONE";
    case FAULT_ACTION_DEGRADE:  return "DEGRADE";
    case FAULT_ACTION_SHUTDOWN: return "SHUTDOWN";
    }
    return "UNKNOWN";
}

} // namespace jetecu
