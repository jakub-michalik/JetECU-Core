#ifndef ECU_MAP_H
#define ECU_MAP_H

#include <stdint.h>
#include <stddef.h>

/* 1D interpolation table */
#define MAP_MAX_POINTS 16

typedef struct {
    float x[MAP_MAX_POINTS];
    float y[MAP_MAX_POINTS];
    int   count;
} ecu_map1d_t;

/* 2D interpolation table (bilinear) */
#define MAP2D_MAX_ROWS 8
#define MAP2D_MAX_COLS 8

typedef struct {
    float row_keys[MAP2D_MAX_ROWS];   /* Y axis */
    float col_keys[MAP2D_MAX_COLS];   /* X axis */
    float values[MAP2D_MAX_ROWS][MAP2D_MAX_COLS];
    int   rows;
    int   cols;
} ecu_map2d_t;

/* 1D lookup with linear interpolation */
float ecu_map1d_lookup(const ecu_map1d_t *map, float x);

/* 2D lookup with bilinear interpolation */
float ecu_map2d_lookup(const ecu_map2d_t *map, float row_key, float col_key);

#endif /* ECU_MAP_H */
