#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_scheduler.h"
}

static int counter_a = 0;
static int counter_b = 0;

static void task_a(void *ctx) { (void)ctx; counter_a++; }
static void task_b(void *ctx) { (void)ctx; counter_b++; }

TEST(Scheduler, Basic) {
    ecu_scheduler_t sched;
    ecu_sched_init(&sched);

    counter_a = 0;
    counter_b = 0;

    ecu_sched_add(&sched, "task_a", task_a, NULL, SCHED_PRIO_HIGH, 10);
    ecu_sched_add(&sched, "task_b", task_b, NULL, SCHED_PRIO_LOW, 20);

    ecu_sched_tick(&sched, 0);
    EXPECT_EQ(counter_a, 1);
    EXPECT_EQ(counter_b, 1);

    ecu_sched_tick(&sched, 10);
    EXPECT_EQ(counter_a, 2);
    EXPECT_EQ(counter_b, 1);

    ecu_sched_tick(&sched, 20);
    EXPECT_EQ(counter_a, 3);
    EXPECT_EQ(counter_b, 2);
}

TEST(Scheduler, PriorityOrder) {
    ecu_scheduler_t sched;
    ecu_sched_init(&sched);

    ecu_sched_add(&sched, "low", task_b, NULL, SCHED_PRIO_LOW, 10);
    ecu_sched_add(&sched, "crit", task_a, NULL, SCHED_PRIO_CRITICAL, 10);

    EXPECT_EQ(sched.tasks[0].priority, SCHED_PRIO_CRITICAL);
    EXPECT_EQ(sched.tasks[1].priority, SCHED_PRIO_LOW);
}

TEST(Scheduler, NoOverrun) {
    ecu_scheduler_t sched;
    ecu_sched_init(&sched);

    counter_a = 0;
    ecu_sched_add(&sched, "fast", task_a, NULL, SCHED_PRIO_HIGH, 10);
    ecu_sched_tick(&sched, 0);

    EXPECT_FALSE(ecu_sched_any_overrun(&sched));
}

/* ---- C++ wrapper tests ---- */

#include "cpp/jetecu/Scheduler.h"

TEST(SchedulerWrapper, AddAndTick) {
    jetecu::Scheduler sched;
    int counter = 0;

    sched.add("test_task", [&]() { counter++; },
              SCHED_PRIO_HIGH, 10);

    sched.tick(0);
    EXPECT_EQ(counter, 1);

    sched.tick(10);
    EXPECT_EQ(counter, 2);
}

TEST(SchedulerWrapper, NoOverrun) {
    jetecu::Scheduler sched;
    sched.add("fast", []() {}, SCHED_PRIO_HIGH, 10);
    sched.tick(0);
    EXPECT_FALSE(sched.anyOverrun());
}
