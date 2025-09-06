#include "test/test_runner.h"
#include "core/ecu_map.h"

static int float_eq(float a, float b, float tol)
{
    float diff = a - b;
    if (diff < 0) diff = -diff;
    return diff < tol;
}

TEST(test_map1d_basic)
{
    ecu_map1d_t map = {
        .x = { 0, 50, 100 },
        .y = { 0, 25, 100 },
        .count = 3,
    };

    ASSERT(float_eq(ecu_map1d_lookup(&map, 0), 0.0f, 0.1f));
    ASSERT(float_eq(ecu_map1d_lookup(&map, 50), 25.0f, 0.1f));
    ASSERT(float_eq(ecu_map1d_lookup(&map, 100), 100.0f, 0.1f));
}

TEST(test_map1d_interpolation)
{
    ecu_map1d_t map = {
        .x = { 0, 100 },
        .y = { 0, 200 },
        .count = 2,
    };

    /* midpoint should interpolate to 100 */
    ASSERT(float_eq(ecu_map1d_lookup(&map, 50), 100.0f, 0.1f));
    ASSERT(float_eq(ecu_map1d_lookup(&map, 25), 50.0f, 0.1f));
}

TEST(test_map1d_clamp)
{
    ecu_map1d_t map = {
        .x = { 10, 90 },
        .y = { 5, 45 },
        .count = 2,
    };

    /* Below range -> clamp to first */
    ASSERT(float_eq(ecu_map1d_lookup(&map, -100), 5.0f, 0.1f));
    /* Above range -> clamp to last */
    ASSERT(float_eq(ecu_map1d_lookup(&map, 200), 45.0f, 0.1f));
}

TEST(test_map2d_basic)
{
    ecu_map2d_t map = {
        .row_keys = { 0, 100 },
        .col_keys = { 0, 100 },
        .values = {
            { 0,  10 },
            { 20, 30 },
        },
        .rows = 2,
        .cols = 2,
    };

    ASSERT(float_eq(ecu_map2d_lookup(&map, 0, 0), 0.0f, 0.1f));
    ASSERT(float_eq(ecu_map2d_lookup(&map, 0, 100), 10.0f, 0.1f));
    ASSERT(float_eq(ecu_map2d_lookup(&map, 100, 0), 20.0f, 0.1f));
    ASSERT(float_eq(ecu_map2d_lookup(&map, 100, 100), 30.0f, 0.1f));
}

TEST(test_map2d_bilinear)
{
    ecu_map2d_t map = {
        .row_keys = { 0, 100 },
        .col_keys = { 0, 100 },
        .values = {
            { 0,  10 },
            { 20, 30 },
        },
        .rows = 2,
        .cols = 2,
    };

    /* Center should be average of all four: (0+10+20+30)/4 = 15 */
    float center = ecu_map2d_lookup(&map, 50, 50);
    ASSERT(float_eq(center, 15.0f, 0.5f));
}

int main(void)
{
    printf("=== Map Tests ===\n");
    RUN_TEST(test_map1d_basic);
    RUN_TEST(test_map1d_interpolation);
    RUN_TEST(test_map1d_clamp);
    RUN_TEST(test_map2d_basic);
    RUN_TEST(test_map2d_bilinear);
    TEST_SUMMARY();
}
