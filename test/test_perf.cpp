#include <gtest/gtest.h>
#include <ctime>

extern "C" {
#include "core/ecu_core.h"
}

TEST(Performance, StepTime) {
    ecu_t ecu;
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ecu_init(&ecu, &cfg);

    ecu_inputs_t in = { 50000, 600, 80 };

    /* Warm up */
    for (int i = 0; i < 1000; i++) {
        ecu_step(&ecu, &in, 0.001f);
    }

    struct timespec start, end;
    int iterations = 100000;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < iterations; i++) {
        ecu_step(&ecu, &in, 0.001f);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    double us_per_step = elapsed / iterations * 1e6;

    printf("\n    %d iterations in %.3f ms (%.2f us/step)", iterations,
           elapsed * 1000.0, us_per_step);

    EXPECT_LT(us_per_step, 100.0);
}
