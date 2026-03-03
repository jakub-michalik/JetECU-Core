/**
 * @file ecu_map.h
 * @brief 1-D and 2-D interpolation lookup tables.
 */

#ifndef ECU_MAP_H
#define ECU_MAP_H

#include <stdint.h>
#include <stddef.h>

/**
 * @defgroup ecu_map Lookup Maps
 * @brief Linear and bilinear interpolation tables for fuel/EGT scheduling.
 * @{
 */

#define MAP_MAX_POINTS 16  /**< Maximum breakpoints in a 1-D map. */

/** @brief 1-D interpolation table (x → y). */
typedef struct {
    float x[MAP_MAX_POINTS];  /**< X-axis breakpoints (ascending). */
    float y[MAP_MAX_POINTS];  /**< Corresponding Y values. */
    int   count;              /**< Number of populated breakpoints. */
} ecu_map1d_t;

#define MAP2D_MAX_ROWS 8  /**< Maximum row keys in a 2-D map. */
#define MAP2D_MAX_COLS 8  /**< Maximum column keys in a 2-D map. */

/** @brief 2-D interpolation table (row, col → value). */
typedef struct {
    float row_keys[MAP2D_MAX_ROWS];  /**< Y-axis keys (ascending). */
    float col_keys[MAP2D_MAX_COLS];  /**< X-axis keys (ascending). */
    float values[MAP2D_MAX_ROWS][MAP2D_MAX_COLS]; /**< Table values. */
    int   rows;  /**< Number of row keys. */
    int   cols;  /**< Number of column keys. */
} ecu_map2d_t;

/**
 * @brief 1-D lookup with linear interpolation.
 *
 * Values outside the table range are clamped to the nearest endpoint.
 *
 * @param[in] map  Pointer to a 1-D map.
 * @param[in] x    Input value.
 * @return Interpolated output.
 */
float ecu_map1d_lookup(const ecu_map1d_t *map, float x);

/**
 * @brief 2-D lookup with bilinear interpolation.
 *
 * @param[in] map      Pointer to a 2-D map.
 * @param[in] row_key  Row (Y-axis) input.
 * @param[in] col_key  Column (X-axis) input.
 * @return Interpolated output.
 */
float ecu_map2d_lookup(const ecu_map2d_t *map, float row_key, float col_key);

/** @} */ /* end ecu_map */

#endif /* ECU_MAP_H */
