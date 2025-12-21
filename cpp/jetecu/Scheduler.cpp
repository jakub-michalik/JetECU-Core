#include "cpp/jetecu/Scheduler.h"

namespace jetecu {

Scheduler::Scheduler()
{
    ecu_sched_init(&sched_);
}

int Scheduler::add(const std::string &name, std::function<void()> fn,
                   sched_priority_t prio, uint32_t interval_ms)
{
    if (binding_count_ >= kMaxTasks) return -1;
    bindings_[binding_count_].fn = std::move(fn);
    int result = ecu_sched_add(&sched_, name.c_str(), trampoline,
                               &bindings_[binding_count_], prio, interval_ms);
    binding_count_++;
    return result;
}

void Scheduler::tick(uint32_t now_ms)
{
    ecu_sched_tick(&sched_, now_ms);
}

bool Scheduler::anyOverrun() const
{
    return ecu_sched_any_overrun(&sched_);
}

uint32_t Scheduler::wcet(int task_idx) const
{
    return ecu_sched_get_wcet(&sched_, task_idx);
}

void Scheduler::trampoline(void *ctx)
{
    auto *binding = static_cast<TaskBinding *>(ctx);
    if (binding->fn) binding->fn();
}

} // namespace jetecu
