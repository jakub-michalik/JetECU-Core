#include "test/test_runner.h"
#include "core/ecu_scheduler.h"

static int counter_a = 0;
static int counter_b = 0;

static void task_a(void *ctx) { (void)ctx; counter_a++; }
static void task_b(void *ctx) { (void)ctx; counter_b++; }

TEST(test_sched_basic)
{
    ecu_scheduler_t sched;
    ecu_sched_init(&sched);

    counter_a = 0;
    counter_b = 0;

    ecu_sched_add(&sched, "task_a", task_a, NULL, SCHED_PRIO_HIGH, 10);
    ecu_sched_add(&sched, "task_b", task_b, NULL, SCHED_PRIO_LOW, 20);

    /* Tick at t=0 */
    ecu_sched_tick(&sched, 0);
    ASSERT_EQ(counter_a, 1);
    ASSERT_EQ(counter_b, 1);

    /* Tick at t=10 — only A should run */
    ecu_sched_tick(&sched, 10);
    ASSERT_EQ(counter_a, 2);
    ASSERT_EQ(counter_b, 1);

    /* Tick at t=20 — both */
    ecu_sched_tick(&sched, 20);
    ASSERT_EQ(counter_a, 3);
    ASSERT_EQ(counter_b, 2);
}

TEST(test_sched_priority_order)
{
    ecu_scheduler_t sched;
    ecu_sched_init(&sched);

    /* Add low prio first, then critical */
    ecu_sched_add(&sched, "low", task_b, NULL, SCHED_PRIO_LOW, 10);
    ecu_sched_add(&sched, "crit", task_a, NULL, SCHED_PRIO_CRITICAL, 10);

    /* Critical should be sorted first */
    ASSERT_EQ(sched.tasks[0].priority, SCHED_PRIO_CRITICAL);
    ASSERT_EQ(sched.tasks[1].priority, SCHED_PRIO_LOW);
}

TEST(test_sched_no_overrun)
{
    ecu_scheduler_t sched;
    ecu_sched_init(&sched);

    counter_a = 0;
    ecu_sched_add(&sched, "fast", task_a, NULL, SCHED_PRIO_HIGH, 10);
    ecu_sched_tick(&sched, 0);

    ASSERT(!ecu_sched_any_overrun(&sched));
}

int main(void)
{
    printf("=== Scheduler Tests ===\n");
    RUN_TEST(test_sched_basic);
    RUN_TEST(test_sched_priority_order);
    RUN_TEST(test_sched_no_overrun);
    TEST_SUMMARY();
}
