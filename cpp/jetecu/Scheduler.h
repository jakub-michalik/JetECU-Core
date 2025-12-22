#ifndef JETECU_SCHEDULER_H
#define JETECU_SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_scheduler.h"
#ifdef __cplusplus
}
#endif

#include <functional>
#include <string>

namespace jetecu {

class Scheduler {
public:
    Scheduler();

    int add(const std::string &name, std::function<void()> fn,
            sched_priority_t prio, uint32_t interval_ms);

    void tick(uint32_t now_ms);
    bool anyOverrun() const;
    uint32_t wcet(int task_idx) const;

    ecu_scheduler_t &raw() { return sched_; }
    const ecu_scheduler_t &raw() const { return sched_; }

private:
    ecu_scheduler_t sched_;

    struct TaskBinding {
        std::function<void()> fn;
    };

    static constexpr int kMaxTasks = SCHED_MAX_TASKS;
    TaskBinding bindings_[kMaxTasks];
    int binding_count_ = 0;

    static void trampoline(void *ctx);
};

} // namespace jetecu

#endif // JETECU_SCHEDULER_H
