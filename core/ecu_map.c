#include "core/ecu_map.h"

float ecu_map1d_lookup(const ecu_map1d_t *map, float x)
{
    if (map->count < 1) return 0.0f;
    if (map->count == 1) return map->y[0];

    if (x <= map->x[0]) return map->y[0];
    if (x >= map->x[map->count - 1]) return map->y[map->count - 1];

    for (int i = 0; i < map->count - 1; i++) {
        if (x >= map->x[i] && x <= map->x[i + 1]) {
            float dx = map->x[i + 1] - map->x[i];
            if (dx < 1e-6f) return map->y[i];
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

static void find_interval(const float *keys, int count, float key,
                          int *i0, int *i1, float *t)
{
    *t = 0.0f;
    if (count < 2 || key <= keys[0]) {
        *i0 = *i1 = 0;
        return;
    }
    if (key >= keys[count - 1]) {
        *i0 = *i1 = count - 1;
        return;
    }
    for (int i = 0; i < count - 1; i++) {
        if (key >= keys[i] && key <= keys[i + 1]) {
            *i0 = i;
            *i1 = i + 1;
            float dk = keys[i + 1] - keys[i];
            *t = (dk > 1e-6f) ? (key - keys[i]) / dk : 0.0f;
            return;
        }
    }
    *i0 = *i1 = count - 1;
}

float ecu_map2d_lookup(const ecu_map2d_t *map, float row_key, float col_key)
{
    if (map->rows < 1 || map->cols < 1) return 0.0f;

    int r0, r1, c0, c1;
    float rt, ct;

    find_interval(map->row_keys, map->rows, row_key, &r0, &r1, &rt);
    find_interval(map->col_keys, map->cols, col_key, &c0, &c1, &ct);

    float top = lerp(map->values[r0][c0], map->values[r0][c1], ct);
    float bot = lerp(map->values[r1][c0], map->values[r1][c1], ct);
    return lerp(top, bot, rt);
}
