#pragma once

#ifndef JETECU_FAULT_MANAGER_H
#define JETECU_FAULT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_fault.h"
#ifdef __cplusplus
}
#endif

#include <cstdint>
#include <iterator>

namespace jetecu {

/** @brief C++ view of an ecu_fault_t record. */
struct FaultRecord {
    uint16_t             code;
    ecu_fault_severity_t severity;
    ecu_fault_action_t   action;
    uint32_t             timestamp;
    bool                 latched;
    bool                 active;
    uint8_t              retryCount;
    uint8_t              maxRetries;

    /** @brief Construct from a C fault record. */
    static FaultRecord from(const ecu_fault_t &f)
    {
        return {f.code, f.severity, f.action, f.timestamp,
                f.latched, f.active, f.retry_count, f.max_retries};
    }
};

/**
 * @brief Lightweight iterator over a contiguous array of ecu_fault_t.
 *
 * Yields FaultRecord values when dereferenced.
 */
class FaultIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = FaultRecord;
    using difference_type = std::ptrdiff_t;
    using pointer = const FaultRecord *;
    using reference = FaultRecord;

    explicit FaultIterator(const ecu_fault_t *ptr) : ptr_(ptr) {}

    FaultRecord operator*() const { return FaultRecord::from(*ptr_); }
    FaultIterator &operator++() { ++ptr_; return *this; }
    FaultIterator operator++(int) { auto tmp = *this; ++ptr_; return tmp; }
    bool operator==(const FaultIterator &o) const { return ptr_ == o.ptr_; }
    bool operator!=(const FaultIterator &o) const { return ptr_ != o.ptr_; }

private:
    const ecu_fault_t *ptr_;
};

/** @brief Range adapter for iterating active faults. */
class ActiveFaultRange {
public:
    ActiveFaultRange(const ecu_fault_t *data, int count)
        : data_(data), count_(count) {}

    FaultIterator begin() const { return FaultIterator(data_); }
    FaultIterator end() const { return FaultIterator(data_ + count_); }
    int size() const { return count_; }

private:
    const ecu_fault_t *data_;
    int count_;
};

/** @brief Range adapter for iterating the fault log. */
class LogRange {
public:
    LogRange(const ecu_fault_t *data, int count)
        : data_(data), count_(count) {}

    FaultIterator begin() const { return FaultIterator(data_); }
    FaultIterator end() const { return FaultIterator(data_ + count_); }
    int size() const { return count_; }

private:
    const ecu_fault_t *data_;
    int count_;
};

class FaultManager {
public:
    FaultManager();

    void report(uint16_t code, ecu_fault_severity_t severity,
                ecu_fault_action_t action, uint32_t now);
    void clear(uint16_t code);
    bool isActive(uint16_t code) const;
    int count() const;
    ecu_fault_action_t action() const;
    bool tryRetry(uint16_t code);
    void setRetries(uint16_t code, uint8_t max_retries);

    /** @brief Range of currently active faults for range-based for. */
    ActiveFaultRange activeFaults() const;

    /** @brief Range of logged faults for range-based for. */
    LogRange log() const;

    /** @brief Number of entries in the fault log. */
    int logCount() const;

    /** @brief Human-readable name for a severity level. */
    static const char *severityName(ecu_fault_severity_t sev);

    /** @brief Human-readable name for a fault action. */
    static const char *actionName(ecu_fault_action_t act);

    ecu_fault_mgr_t &raw() { return mgr_; }
    const ecu_fault_mgr_t &raw() const { return mgr_; }

private:
    ecu_fault_mgr_t mgr_;
};

} // namespace jetecu

#endif // JETECU_FAULT_MANAGER_H
