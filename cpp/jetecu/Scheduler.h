#pragma once

#ifndef JETECU_SCHEDULER_H
#define JETECU_SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_scheduler.h"
#ifdef __cplusplus
}
#endif

#include <cstdint>
#include <functional>
#include <iterator>
#include <string>

namespace jetecu {

/** @brief Snapshot of a registered task's state. */
struct TaskInfo {
    const char      *name;
    sched_priority_t priority;
    uint32_t         interval_ms;
    uint32_t         worst_case_us;
    uint32_t         run_count;
    bool             overrun;

    /** @brief Construct from a C task descriptor. */
    static TaskInfo from(const sched_task_t &t)
    {
        return {t.name, t.priority, t.interval_ms,
                t.worst_case_us, t.run_count, t.overrun};
    }
};

/**
 * @brief Iterator over registered scheduler tasks.
 *
 * Yields TaskInfo values when dereferenced.
 */
class TaskIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = TaskInfo;
    using difference_type = std::ptrdiff_t;
    using pointer = const TaskInfo *;
    using reference = TaskInfo;

    explicit TaskIterator(const sched_task_t *ptr) : ptr_(ptr) {}

    TaskInfo operator*() const { return TaskInfo::from(*ptr_); }
    TaskIterator &operator++() { ++ptr_; return *this; }
    TaskIterator operator++(int) { auto tmp = *this; ++ptr_; return tmp; }
    bool operator==(const TaskIterator &o) const { return ptr_ == o.ptr_; }
    bool operator!=(const TaskIterator &o) const { return ptr_ != o.ptr_; }

private:
    const sched_task_t *ptr_;
};

/** @brief Range adapter for iterating registered tasks. */
class TaskRange {
public:
    TaskRange(const sched_task_t *data, int count)
        : data_(data), count_(count) {}

    TaskIterator begin() const { return TaskIterator(data_); }
    TaskIterator end() const { return TaskIterator(data_ + count_); }
    int size() const { return count_; }

private:
    const sched_task_t *data_;
    int count_;
};

class Scheduler {
public:
    Scheduler();

    int add(const std::string &name, std::function<void()> fn,
            sched_priority_t prio, uint32_t interval_ms);

    void tick(uint32_t now_ms);
    bool anyOverrun() const;
    uint32_t wcet(int task_idx) const;

    /** @brief Number of registered tasks. */
    int taskCount() const;

    /** @brief Get info snapshot of task at index. */
    TaskInfo taskInfo(int idx) const;

    /** @brief Range of registered tasks for range-based for. */
    TaskRange tasks() const;

    ecu_scheduler_t &raw() { return sched_; }
    const ecu_scheduler_t &raw() const { return sched_; }

private:
    ecu_scheduler_t sched_;

    struct TaskBinding {
        std::function<void()> fn;
        std::string name;
    };

    static constexpr int kMaxTasks = SCHED_MAX_TASKS;
    TaskBinding bindings_[kMaxTasks];
    int binding_count_ = 0;

    static void trampoline(void *ctx);
};

} // namespace jetecu

#endif // JETECU_SCHEDULER_H
