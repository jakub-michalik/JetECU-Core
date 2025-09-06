#include "core/ecu_map.h"

float ecu_map1d_lookup(const ecu_map1d_t *map, float x)
{
    if (map->count < 1) return 0.0f;
    if (map->count == 1) return map->y[0];

    /* Clamp below */
    if (x <= map->x[0]) return map->y[0];
    /* Clamp above */
    if (x >= map->x[map->count - 1]) return map->y[map->count - 1];

    /* Find interval */
    for (int i = 0; i < map->count - 1; i++) {
        if (x >= map->x[i] && x <= map->x[i + 1]) {
            float dx = map->x[i + 1] - map->x[i];
            if (dx < 0.0001f) return map->y[i];
            float t = (x - map->x[i]) / dx;
            return map->y[i] + t * (map->y[i + 1] - map->y[i]);
        }
    }

    return map->y[map->count - 1];
}

static float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float ecu_map2d_lookup(const ecu_map2d_t *map, float row_key, float col_key)
{
    if (map->rows < 1 || map->cols < 1) return 0.0f;

    /* Find row interval */
    int r0 = 0, r1 = 0;
    float rt = 0.0f;
    if (row_key <= map->row_keys[0]) {
        r0 = r1 = 0;
    } else if (row_key >= map->row_keys[map->rows - 1]) {
        r0 = r1 = map->rows - 1;
    } else {
        for (int i = 0; i < map->rows - 1; i++) {
            if (row_key >= map->row_keys[i] && row_key <= map->row_keys[i + 1]) {
                r0 = i;
                r1 = i + 1;
                float dr = map->row_keys[r1] - map->row_keys[r0];
                rt = (dr > 0.0001f) ? (row_key - map->row_keys[r0]) / dr : 0.0f;
                break;
            }
        }
    }

    /* Find col interval */
    int c0 = 0, c1 = 0;
    float ct = 0.0f;
    if (col_key <= map->col_keys[0]) {
        c0 = c1 = 0;
    } else if (col_key >= map->col_keys[map->cols - 1]) {
        c0 = c1 = map->cols - 1;
    } else {
        for (int i = 0; i < map->cols - 1; i++) {
            if (col_key >= map->col_keys[i] && col_key <= map->col_keys[i + 1]) {
                c0 = i;
                c1 = i + 1;
                float dc = map->col_keys[c1] - map->col_keys[c0];
                ct = (dc > 0.0001f) ? (col_key - map->col_keys[c0]) / dc : 0.0f;
                break;
            }
        }
    }

    /* Bilinear interpolation */
    float v00 = map->values[r0][c0];
    float v01 = map->values[r0][c1];
    float v10 = map->values[r1][c0];
    float v11 = map->values[r1][c1];

    float top = lerp(v00, v01, ct);
    float bot = lerp(v10, v11, ct);
    return lerp(top, bot, rt);
}
