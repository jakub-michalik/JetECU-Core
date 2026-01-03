#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_map.h"
}

TEST(Map1D, Basic) {
    ecu_map1d_t map = {};
    map.x[0] = 0; map.x[1] = 50; map.x[2] = 100;
    map.y[0] = 0; map.y[1] = 25; map.y[2] = 100;
    map.count = 3;

    EXPECT_NEAR(ecu_map1d_lookup(&map, 0), 0.0f, 0.1f);
    EXPECT_NEAR(ecu_map1d_lookup(&map, 50), 25.0f, 0.1f);
    EXPECT_NEAR(ecu_map1d_lookup(&map, 100), 100.0f, 0.1f);
}

TEST(Map1D, Interpolation) {
    ecu_map1d_t map = {};
    map.x[0] = 0; map.x[1] = 100;
    map.y[0] = 0; map.y[1] = 200;
    map.count = 2;

    EXPECT_NEAR(ecu_map1d_lookup(&map, 50), 100.0f, 0.1f);
    EXPECT_NEAR(ecu_map1d_lookup(&map, 25), 50.0f, 0.1f);
}

TEST(Map1D, Clamp) {
    ecu_map1d_t map = {};
    map.x[0] = 10; map.x[1] = 90;
    map.y[0] = 5;  map.y[1] = 45;
    map.count = 2;

    EXPECT_NEAR(ecu_map1d_lookup(&map, -100), 5.0f, 0.1f);
    EXPECT_NEAR(ecu_map1d_lookup(&map, 200), 45.0f, 0.1f);
}

TEST(Map2D, Basic) {
    ecu_map2d_t map = {};
    map.row_keys[0] = 0; map.row_keys[1] = 100;
    map.col_keys[0] = 0; map.col_keys[1] = 100;
    map.values[0][0] = 0;  map.values[0][1] = 10;
    map.values[1][0] = 20; map.values[1][1] = 30;
    map.rows = 2; map.cols = 2;

    EXPECT_NEAR(ecu_map2d_lookup(&map, 0, 0), 0.0f, 0.1f);
    EXPECT_NEAR(ecu_map2d_lookup(&map, 0, 100), 10.0f, 0.1f);
    EXPECT_NEAR(ecu_map2d_lookup(&map, 100, 0), 20.0f, 0.1f);
    EXPECT_NEAR(ecu_map2d_lookup(&map, 100, 100), 30.0f, 0.1f);
}

TEST(Map2D, Bilinear) {
    ecu_map2d_t map = {};
    map.row_keys[0] = 0; map.row_keys[1] = 100;
    map.col_keys[0] = 0; map.col_keys[1] = 100;
    map.values[0][0] = 0;  map.values[0][1] = 10;
    map.values[1][0] = 20; map.values[1][1] = 30;
    map.rows = 2; map.cols = 2;

    float center = ecu_map2d_lookup(&map, 50, 50);
    EXPECT_NEAR(center, 15.0f, 0.5f);
}

/* ---- C++ wrapper tests ---- */

#include "cpp/jetecu/Map.h"

TEST(Map1DWrapper, Lookup) {
    jetecu::Map1D map({{0, 0}, {50, 25}, {100, 100}});
    EXPECT_NEAR(map.lookup(0), 0.0f, 0.1f);
    EXPECT_NEAR(map.lookup(50), 25.0f, 0.1f);
    EXPECT_NEAR(map.lookup(100), 100.0f, 0.1f);
}

TEST(Map1DWrapper, Interpolation) {
    jetecu::Map1D map({{0, 0}, {100, 200}});
    EXPECT_NEAR(map.lookup(50), 100.0f, 0.1f);
}

TEST(Map2DWrapper, Lookup) {
    jetecu::Map2D map({0, 100}, {0, 100},
                      {{0, 10}, {20, 30}});
    EXPECT_NEAR(map.lookup(0, 0), 0.0f, 0.1f);
    EXPECT_NEAR(map.lookup(50, 50), 15.0f, 0.5f);
}
